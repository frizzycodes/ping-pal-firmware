#include "hardware/Button.h"
#include "PingPalApp.h"
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
    updateLedForState(stateMachine.getCurrentState());
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

// Simulated events

void PingPalApp::onButtonLongPress()
{
    State s = stateMachine.getCurrentState();

    if (s == State::SETUP_MODE) // Prevent in Setup_Mode
        return;

    if (setupConfirmationPending)
    {
        setupConfirmationPending = false;
        stateMachine.transitionTo(State::SETUP_MODE);
        updateLedForState(stateMachine.getCurrentState());
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
void PingPalApp::onWiFiConnected() {}
void PingPalApp::onWiFiDisconnected()
{
}
void PingPalApp::onPingSuccess()
{
    stateMachine.transitionTo(State::ONLINE_PING_OK);
    updateLedForState(stateMachine.getCurrentState());
}
void PingPalApp::onPingFail()
{
    stateMachine.transitionTo(State::ONLINE_PING_FAIL);
    updateLedForState(stateMachine.getCurrentState());
}
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
    default:
        break;
    }
}