#pragma once
#include <Arduino.h>

class OledDisplay
{
private:
    uint8_t bootFrameIndex;
    unsigned long lastBootFrameTime;
    unsigned long bootStartTime;
    bool bootAnimationDone;

    static constexpr unsigned long BOOT_FRAME_INTERVAL_MS = 300;
    static constexpr unsigned long BOOT_MIN_TIME_MS = 2000;
    static constexpr uint8_t BOOT_FRAME_COUNT = 5;

    void drawBootFrame(uint8_t frame);

public:
    void begin();
    void clear();

    // BOOT animation
    void onBootEnter();
    void updateBoot();
    bool isBootDone() const;

    // Other UI states
    void drawConnectingWiFi();
    void drawWiFiConnected();
    void drawWiFiDisconnected();
    void drawOnlinePinging();
    void drawPingSuccess();
    void drawPingFail();
    void drawSetupMode(const String &ip);
    void drawSetupConfirmation();
    void drawError();
};
