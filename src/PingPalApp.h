#pragma once
#include "StateMachine.h"
#include "PingPalApp.h"

class PingPalApp
{
private:
    StateMachine stateMachine;
    Button button;
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
};
