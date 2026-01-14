#pragma once
#include "StateMachine.h"
#include "PingService.h"
#include "hardware/Button.h"
#include "hardware/Led.h"
#include "hardware/OledDisplay.h"
#include <WebServer.h>
#include <Preferences.h>

class PingPalApp
{
private:
    StateMachine stateMachine;
    Button button;
    PingService pingService;
    Led led;
    OledDisplay oled;
    WebServer setupServer;
    Preferences preferences;
    bool setupConfirmationPending;

    // Actions (initiated by app)
    void updateLedForState(State state);
    void onStateEntered(State newState);
    void startWiFiConnection();

    unsigned long resultDisplayStartTime;
    static constexpr unsigned long RESULT_DISPLAY_MS = 500;

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

    // Setup Mode Events
    void startSetupAP();
    void stopSetupAP();
};
