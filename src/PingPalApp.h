#pragma once
#include "StateMachine.h"
#include "PingPalApp.h"
#include "hardware/Button.h"
#include "hardware/Led.h"

class PingPalApp
{
private:
    StateMachine stateMachine;
    Button button;
    Led led;
    bool setupConfirmationPending;

public:
    PingPalApp();
    void setup();
    void loop();
    void onButtonShortPress();
    void onButtonLongPress();
    void onWiFiConnected();
    void onWiFiDisconnected();
    void onPingSuccess();
    void onPingFail();
    void updateLedForState(State state);
};
