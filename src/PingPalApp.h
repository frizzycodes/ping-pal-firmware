#pragma once
#include "StateMachine.h"
#include "hardware/Button.h"
#include "hardware/Led.h"

class PingPalApp
{
private:
    StateMachine stateMachine;
    Button button;
    Led led;
    bool setupConfirmationPending;

    // Actions (initiated by app)
    void updateLedForState(State state);
    void onStateEntered(State newState);
    void startWiFiConnection();
    void startPing();

public:
    PingPalApp();

    void setup();
    void loop();

    // Button events
    void onButtonShortPress();
    void onButtonLongPress();

    // Wi-Fi events
    void onWiFiConnected();
    void onWiFiDisconnected();

    // Ping events
    void onPingSuccess();
    void onPingFail();
};
