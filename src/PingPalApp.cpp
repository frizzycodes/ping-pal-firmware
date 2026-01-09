#include "hardware/Button.h"
#include <stdio.h>

PingPalApp::PingPalApp()
    : button(5),// BUTTON_PIN {}
      setupConfirmationPending(false)
{
}

void PingPalApp::setup()
{
    button.begin();
    printf("\n PingPal Starting...");
}

void PingPalApp::loop()
{   
    ButtonEvent e = button.update();
    if(e == ButtonEvent::SHORT_PRESS){
        onButtonShortPress();
    }
    if(e == ButtonEvent::LONG_PRESS){
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
void PingPalApp::onWiFiDisconnected() {}
void PingPalApp::onPingSuccess()
{
    stateMachine.transitionTo(State::ONLINE_PING_OK);
}
void PingPalApp::onPingFail()
{
    stateMachine.transitionTo(State::ONLINE_PING_FAIL);
}