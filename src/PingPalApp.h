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
    bool setupServerRunning = false;
    State prevStateBeforeConfirmation;

    // Actions (initiated by app)
    void updateLedForState(State state);
    void onStateEntered(State newState);
    void startWiFiConnection();

    unsigned long resultDisplayStartTime;
    static constexpr unsigned long RESULT_DISPLAY_MS = 500;

    unsigned long lastWiFiRetryTime = 0;
    uint8_t wifiRetryCount = 0;

    static constexpr unsigned long WIFI_RETRY_INTERVAL_MS = 8000;
    static constexpr uint8_t WIFI_MAX_RETRIES = 5;

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
    void onOnlinePinging();
    void onPingSuccess();
    void onPingFail();

    // Setup Mode Events
    void startSetupAP();
    void stopSetupAP();
};
