#include "hardware/Button.h"
#include "PingPalApp.h"
#include "PingService.h"
#include <WiFi.h>
#include <WiFiGeneric.h>
#include <stdio.h>

PingPalApp::PingPalApp()
    : button(5),       // BUTTON_PIN {}
      led(23, 19, 18), // LED_PIN {}
      setupServer(80), // PORT
      setupConfirmationPending(false),
      resultDisplayStartTime(0)
{
}

void PingPalApp::setup()
{
    button.begin();
    led.begin();
    oled.begin();
    onStateEntered(stateMachine.getCurrentState());

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
    }
    if (e == ButtonEvent::LONG_PRESS)
    {
        onButtonLongPress();
    }

    if (pingResult == PingStatus::SUCCESS)
    {
        onPingSuccess();
    }
    else if (pingResult == PingStatus::FAIL)
    {
        onPingFail();
    }

    if (state == State::ONLINE_PING_OK || state == State::ONLINE_PING_FAIL)
    {
        if (millis() - resultDisplayStartTime >= RESULT_DISPLAY_MS)
        {
            stateMachine.transitionTo(State::ONLINE_PINGING);
            onStateEntered(stateMachine.getCurrentState());
        }
    }

    if (stateMachine.getCurrentState() == State::SETUP_MODE)
    {
        setupServer.handleClient();
    }

    if (state == State::BOOT)
    {
        oled.updateBoot();

        if (oled.isBootDone())
        {
            stateMachine.transitionTo(State::CONNECTING_WIFI);
            onStateEntered(State::CONNECTING_WIFI);
        }
    }
}

// Button events
void PingPalApp::onButtonLongPress()
{
    State s = stateMachine.getCurrentState();

    if (s == State::SETUP_MODE) // Save and Exit from SETUP_MODE
    {
        setupConfirmationPending = false;

        // later: save credentials here
        stateMachine.transitionTo(State::BOOT);
        onStateEntered(State::BOOT);
        return;
    }
    if (setupConfirmationPending)
    {
        setupConfirmationPending = false;
        stateMachine.transitionTo(State::SETUP_MODE);
        onStateEntered(stateMachine.getCurrentState());
        return;
    }
    setupConfirmationPending = true;
    oled.drawSetupConfirmation();
}
void PingPalApp::onButtonShortPress()
{
    if (setupConfirmationPending)
    {
        setupConfirmationPending = false;
        // OLED later: "Cancelled"
        return;
    }
}

// Wi-Fi events
void PingPalApp::onWiFiConnected()
{
    stateMachine.transitionTo(State::ONLINE_PINGING);
    onStateEntered(State::ONLINE_PINGING);
}
void PingPalApp::onWiFiDisconnected()
{
    stateMachine.transitionTo(State::WIFI_DISCONNECTED);
    onStateEntered(State::WIFI_DISCONNECTED);
}

// Ping events
void PingPalApp::onPingSuccess()
{
    resultDisplayStartTime = millis();
    stateMachine.transitionTo(State::ONLINE_PING_OK);
    onStateEntered(stateMachine.getCurrentState());
}
void PingPalApp::onPingFail()
{
    resultDisplayStartTime = millis();
    stateMachine.transitionTo(State::ONLINE_PING_FAIL);
    onStateEntered(stateMachine.getCurrentState());
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
    pingService.disable();
    stopSetupAP();

    switch (newState)
    {
    case State::BOOT:
        oled.onBootEnter();
        break;
    case State::SETUP_MODE:
        startSetupAP();
        break;
    case State::CONNECTING_WIFI:
        startWiFiConnection();
        break;
    case State::WIFI_DISCONNECTED:
        break;
    case State::ONLINE_PINGING:
        pingService.enable();
        break;
    case State::ONLINE_PING_OK:
        break;
    case State::ONLINE_PING_FAIL:
        break;
    case State::ERROR_STATE:
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
                stateMachine.transitionTo(State::CONNECTING_WIFI);
                onStateEntered(State::CONNECTING_WIFI);
                break;
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                onWiFiConnected();
                break;
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
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

    setupServer.on("/", HTTP_GET, [this]
                   { setupServer.send(200, "text/html",
                                      "<html>"
                                      "<body>"
                                      "<h2>PingPal Setup</h2>"
                                      "<form method='POST' action='/save'>"
                                      "SSID:<br><input name='ssid'><br>"
                                      "Password:<br><input name='pass' type='password'><br><br>"
                                      "<input type='submit' value='Save'>"
                                      "</form>"
                                      "</body>"
                                      "</html>"); });
    setupServer.on("/save", HTTP_POST, [this]
                   {
                       String ssid = setupServer.arg("ssid");
                       String pass = setupServer.arg("pass");
                       preferences.begin("wifi",false);
                       preferences.putString("ssid", ssid);
                       preferences.putString("pass", pass);
                       preferences.end();
                       setupServer.send(200, "text/plain", "Saved. Rebooting...");

                       delay(1000);
                       stateMachine.transitionTo(State::BOOT);
                       onStateEntered(State::BOOT); });

    setupServer.begin();
}
void PingPalApp::stopSetupAP()
{
    if (WiFi.getMode() == WIFI_AP)
    {
        setupServer.stop();
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_OFF);
    }
}