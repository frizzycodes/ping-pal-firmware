#pragma once
#include "StateMachine.h"

class PingPalApp
{
private:
    StateMachine stateMachine;
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
