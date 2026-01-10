#include "hardware/Button.h"
#include "PingPalApp.h"
#include <WiFi.h>
#include <WiFiType.h>
#include <WiFiGeneric.h>
#include <stdio.h>

PingPalApp::PingPalApp()
    : button(5),       // BUTTON_PIN {}
      led(23, 19, 18), // LED_PIN {}
      setupConfirmationPending(false)
{
}

void PingPalApp::setup()
{
    button.begin();
    led.begin();

    onStateEntered(stateMachine.getCurrentState());

    printf("\n PingPal Starting...");
}

void PingPalApp::loop()
{
    ButtonEvent e = button.update();
    if (e == ButtonEvent::SHORT_PRESS)
    {
        onButtonShortPress();
    }
    if (e == ButtonEvent::LONG_PRESS)
    {
        onButtonLongPress();
    }
    stateMachine.update();
}

// Button events
void PingPalApp::onButtonLongPress()
{
    State s = stateMachine.getCurrentState();

    if (s == State::SETUP_MODE) // Prevent in Setup_Mode
        return;

    if (setupConfirmationPending)
    {
        setupConfirmationPending = false;
        stateMachine.transitionTo(State::SETUP_MODE);
        onStateEntered(stateMachine.getCurrentState());
        return;
    }
    setupConfirmationPending = true;
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
    stateMachine.transitionTo(State::ONLINE_PING_OK);
    onStateEntered(stateMachine.getCurrentState());
}
void PingPalApp::onPingFail()
{
    stateMachine.transitionTo(State::ONLINE_PING_FAIL);
    onStateEntered(stateMachine.getCurrentState());
}

// Actions (initiated by app)

void PingPalApp::updateLedForState(State state)
{
    switch (state)
    {
    case State::BOOT:
        led.setColor(LedColor::BLUE);
        break;
    case State::SETUP_MODE:
        led.setColor(LedColor::YELLOW);
        break;
    case State::CONNECTING_WIFI:
        led.setColor(LedColor::BLUE);
        break;
    case State::ONLINE_PINGING:
        led.setColor(LedColor::BLUE);
        break;
    case State::ONLINE_PING_OK:
        led.setColor(LedColor::GREEN);
        break;
    case State::ONLINE_PING_FAIL:
        led.setColor(LedColor::RED);
        break;
    case State::ERROR_STATE:
        led.setColor(LedColor::RED);
        break;
    default:
        break;
    }
}

void PingPalApp::onStateEntered(State newState)
{
    updateLedForState(newState);
    switch (newState)
    {
    case State::BOOT:
        startWiFiConnection();
        break;
    case State::SETUP_MODE:
        break;
    case State::CONNECTING_WIFI:
        break;
    case State::ONLINE_PINGING:
        startPing();
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
    static bool wifiEventRegistered = false;
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

    // TODO: replace with stored credentials later
    const char *ssid = "YOUR_WIFI";
    const char *password = "YOUR_PASSWORD";
    WiFi.begin(ssid, password);
}
void PingPalApp::startPing() {}
