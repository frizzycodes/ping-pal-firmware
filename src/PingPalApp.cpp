#include "hardware/Button.h"
#include "PingPalApp.h"
#include "PingService.h"
#include <WiFi.h>
#include <WiFiGeneric.h>
#include <stdio.h>

PingPalApp::PingPalApp()
    : button(5),
      led(23, 19, 18),
      setupServer(80),
      resultDisplayStartTime(0),
      lastUiUpdate(0),
      lastResultTime(0)
{
}

void PingPalApp::setup()
{
    button.begin();
    led.begin();
    oled.begin();
    onStateEntered(stateMachine.getCurrentState());
    Serial.begin(115200);

    printf("\n PingPal Starting...");
}

void PingPalApp::loop()
{
    ButtonEvent e = button.update();
    State state = stateMachine.getCurrentState();
    PingStatus pingResult = pingService.update();
    led.update();
    stateMachine.update();

    if (e == ButtonEvent::SHORT_PRESS)
    {
        onButtonShortPress();
        return;
    }
    if (e == ButtonEvent::LONG_PRESS)
    {
        onButtonLongPress();
        return;
    }

    if (pingResult == PingStatus::SUCCESS)
    {
        onPingSuccess();
        return;
    }
    else if (pingResult == PingStatus::FAIL)
    {
        onPingFail();
        return;
    }
    if ((state == State::ONLINE_PING_OK || state == State::ONLINE_PING_FAIL) &&
        WiFi.status() != WL_CONNECTED)
    {
        stateMachine.transitionTo(State::WIFI_DISCONNECTED);
        onStateEntered(State::WIFI_DISCONNECTED);
        return;
    }

    if (state == State::ONLINE_PING_OK || state == State::ONLINE_PING_FAIL)
    {
        if (millis() - lastUiUpdate >= UI_REFRESH_MS)
        {
            lastUiUpdate = millis();

            unsigned long checkTime =
                (millis() - lastResultTime + 1000) / 1000;

            if (state == State::ONLINE_PING_OK)
                oled.drawPingSuccess(
                    WiFi.SSID(),
                    pingService.getLastPingLatency(),
                    checkTime);
            else
                oled.drawPingFail(
                    WiFi.SSID(),
                    checkTime);
        }
        return;
    }

    if (setupServerRunning)
    {
        setupServer.handleClient();
        return;
    }

    if (state == State::BOOT)
    {
        oled.updateBoot();

        if (oled.isBootDone())
        {
            preferences.begin("wifi", true);
            bool hasCreds = preferences.isKey("ssid");
            preferences.end();

            if (hasCreds)
            {
                stateMachine.transitionTo(State::CONNECTING_WIFI);
                oled.drawWiFiConnecting();
                onStateEntered(State::CONNECTING_WIFI);
            }
            else
            {
                stateMachine.transitionTo(State::SETUP_MODE);
                onStateEntered(State::SETUP_MODE);
            }
            return;
        }
    }
    if (state == State::WIFI_DISCONNECTED) // updates wifi disconnected / retrying ui
    {
        unsigned long now = millis();
        if (now - lastWiFiRetryTime >= WIFI_RETRY_INTERVAL_MS)
        {
            lastWiFiRetryTime = now;
            wifiRetryCount++;
            if (wifiRetryCount < WIFI_MAX_RETRIES)
            {
                stateMachine.transitionTo(State::CONNECTING_WIFI);
                onStateEntered(State::CONNECTING_WIFI);
            }
            else
            {
                stateMachine.transitionTo(State::SETUP_CONFIRMATION);
                onStateEntered(State::SETUP_CONFIRMATION);
            }
            return;
        }
    }
}

// Button events
void PingPalApp::onButtonLongPress()
{
    State s = stateMachine.getCurrentState();

    if (s == State::SETUP_MODE)
        return;

    if (s == State::SETUP_CONFIRMATION)
    {
        stateMachine.transitionTo(State::SETUP_MODE);
        onStateEntered(State::SETUP_MODE);
        return;
    }
    prevStateBeforeConfirmation = s;
    stateMachine.transitionTo(State::SETUP_CONFIRMATION);
    onStateEntered(State::SETUP_CONFIRMATION);
}

void PingPalApp::onButtonShortPress()
{
    if (stateMachine.getCurrentState() == State::SETUP_CONFIRMATION)
    {
        stateMachine.transitionTo(prevStateBeforeConfirmation);
        onStateEntered(prevStateBeforeConfirmation);
    }
}

// Wi-Fi events
void PingPalApp::onWiFiConnected()
{
    if (stateMachine.getCurrentState() != State::CONNECTING_WIFI)
        return;
    wifiRetryCount = 0;
    lastWiFiRetryTime = 0;
    stateMachine.transitionTo(State::ONLINE_PINGING);
    onStateEntered(State::ONLINE_PINGING);
}

void PingPalApp::onWiFiDisconnected()
{
    State s = stateMachine.getCurrentState();

    if (s == State::SETUP_MODE || s == State::SETUP_CONFIRMATION)
        return;

    stateMachine.transitionTo(State::WIFI_DISCONNECTED);
    onStateEntered(State::WIFI_DISCONNECTED);
}

// Ping events
void PingPalApp::onPingSuccess()
{
    stateMachine.transitionTo(State::ONLINE_PING_OK);
    onStateEntered(State::ONLINE_PING_OK);
}

void PingPalApp::onPingFail()
{
    stateMachine.transitionTo(State::ONLINE_PING_FAIL);
    onStateEntered(State::ONLINE_PING_FAIL);
}

// Actions (initiated by app)

void PingPalApp::updateLedForState(State state)
{
    switch (state)
    {
    case State::BOOT:
        led.set(LedColor::BLUE, LedPattern::SOLID);
        break;
    case State::SETUP_MODE:
        led.set(LedColor::YELLOW, LedPattern::SOLID);
        break;
    case State::CONNECTING_WIFI:
        led.set(LedColor::BLUE, LedPattern::BLINK_SLOW);
        break;
    case State::WIFI_DISCONNECTED:
        led.set(LedColor::RED, LedPattern::BLINK_SLOW);
        break;
    case State::ONLINE_PINGING:
        led.set(LedColor::PURPLE, LedPattern::SOLID);
        break;
    case State::ONLINE_PING_OK:
        led.set(LedColor::GREEN, LedPattern::SOLID);
        break;
    case State::ONLINE_PING_FAIL:
        led.set(LedColor::RED, LedPattern::SOLID);
        break;
    case State::ERROR_STATE:
        led.set(LedColor::RED, LedPattern::BLINK_FAST);
        break;
    default:
        break;
    }
}

void PingPalApp::onStateEntered(State newState)
{

    updateLedForState(newState);

    // Default safety rule
    stopSetupAP();

    switch (newState)
    {
    case State::BOOT:
        wifiRetryCount = 0;
        lastWiFiRetryTime = 0;
        pingService.disable();
        oled.onBootEnter();
        break;
    case State::SETUP_MODE:
        wifiRetryCount = 0;
        lastWiFiRetryTime = 0;
        WiFi.disconnect(true);
        pingService.disable();
        startSetupAP();
        break;
    case State::SETUP_CONFIRMATION:
        oled.drawSetupConfirmation();
        pingService.disable();
        break;
    case State::CONNECTING_WIFI:
        pingService.disable();
        startWiFiConnection();
        break;
    case State::WIFI_DISCONNECTED:
        lastWiFiRetryTime = millis();
        pingService.disable();
        oled.drawWiFiDisconnected();
        break;
    case State::ONLINE_PINGING:
        onOnlinePinging();
        break;
    case State::ONLINE_PING_OK:
        lastResultTime = millis();
        break;
    case State::ONLINE_PING_FAIL:
        lastResultTime = millis();
        break;
    case State::ERROR_STATE:
        pingService.enable();
        oled.onError();
        break;
    default:
        break;
    }
}
void PingPalApp::startWiFiConnection()
{
    static bool wifiEventRegistered = false; // Wi-Fi events live for the lifetime of the firmware
    if (!wifiEventRegistered)
    {
        WiFi.setHostname("ping-pal");
        WiFi.mode(WIFI_STA);
        WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info)
                     {
            switch (event)
            {
            case ARDUINO_EVENT_WIFI_STA_CONNECTED:
                // optional: OLED update like "Connected, waiting for IP"
                break;
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                onWiFiConnected();
                break;
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                if (stateMachine.getCurrentState() != State::CONNECTING_WIFI &&
                    stateMachine.getCurrentState() != State::ONLINE_PINGING)
                    return;
                onWiFiDisconnected();
                break; 
            default:
                break;
            } });
        wifiEventRegistered = true;
    }

    preferences.begin("wifi", true);
    if (!preferences.isKey("ssid"))
    {
        // First boot or wiped device
        preferences.clear();
        preferences.putUInt("ver", 1);
        preferences.end();

        stateMachine.transitionTo(State::SETUP_MODE);
        return;
    }
    String ssid = preferences.getString("ssid");
    String password = preferences.getString("pass");
    preferences.end();
    WiFi.begin(ssid, password);
}

void PingPalApp::startSetupAP()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);

    static constexpr const char *SETUP_AP_SSID = "PingPal-Setup";
    static constexpr const char *SETUP_AP_PASS = "12345678";

    WiFi.softAP(SETUP_AP_SSID, SETUP_AP_PASS);

    IPAddress ip = WiFi.softAPIP();
    oled.drawSetupMode(ip.toString());
    static String scannedSSIDs[20];
    static int scannedCount = 0;
    WiFi.scanDelete(); // clears previous scan results
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n && i < 20; i++)
    {
        String ssid = WiFi.SSID(i);

        if (ssid.length() > 0)
        {
            scannedSSIDs[scannedCount] = ssid;
            scannedCount++;
        }
    }
    String html = "<html><body>";
    html += "<h2>PingPal Setup</h2>";

    html += "<form method='POST' action='/save'>";
    html += "WiFi Network:<br>";
    html += "<select name='ssid'>";

    for (auto &ssid : scannedSSIDs)
    {
        html += "<option value='" + ssid + "'>" + ssid + "</option>";
    }

    html += "</select><br><br>";

    html += "Password:<br>";
    html += "<input name='pass' type='password'><br><br>";

    html += "<input type='submit' value='Save'>";
    html += "</form>";
    html += "</body></html>";

    setupServer.on("/", HTTP_GET, [this, html]
                   { setupServer.send(200, "text/html", html); });
    setupServer.on("/save", HTTP_POST, [this]
                   {
                       String ssid = setupServer.arg("ssid");
                       String pass = setupServer.arg("pass");
                       preferences.begin("wifi",false);
                       preferences.putString("ssid", ssid);
                       preferences.putString("pass", pass);
                       preferences.end();
                       setupServer.send(200, "text/plain", "Saved. Rebooting...");
                       stateMachine.transitionTo(State::CONNECTING_WIFI);
                       onStateEntered(State::CONNECTING_WIFI); });

    setupServer.begin();
    setupServerRunning = true;
}
void PingPalApp::stopSetupAP()
{
    if (WiFi.getMode() == WIFI_AP)
    {
        setupServer.stop();
        setupServerRunning = false;
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_OFF);
    }
}
void PingPalApp::onOnlinePinging()
{
    String ssid = WiFi.SSID();
    String host = pingService.getTarget();
    oled.drawOnlinePinging(ssid, host);
    pingService.enable();
}
