#pragma once
#include <Arduino.h>

class OledDisplay
{
private:
    // ---- Boot animation ----
    uint8_t bootFrameIndex;
    unsigned long lastBootFrameTime;
    unsigned long bootStartTime;
    bool bootAnimationDone;

    // ---- Loading dots (shared across states) ----
    uint8_t dotCount = 0;
    unsigned long lastDotTime = 0;

    static constexpr uint8_t MAX_DOTS = 5;
    static constexpr unsigned long DOT_INTERVAL_MS = 100; // 1 sec

    void updateDots();
    void drawDots(int x, int y);

    static constexpr unsigned long BOOT_FRAME_INTERVAL_MS = 500;
    static constexpr unsigned long BOOT_MIN_TIME_MS = 4000;
    static constexpr uint8_t BOOT_FRAME_COUNT = 5;

    void drawBootFrame(uint8_t frame);

public:
    OledDisplay();

    void begin();
    void clear();

    // Boot
    void onBootEnter();
    void updateBoot();
    bool isBootDone() const;

    // UI states
    void drawSetupConfirmation();
    void drawSetupMode(const String &ip);
    void drawWiFiDisconnected();

    void drawOnlinePinging(const String &ssid, const String &host);
    void drawPingSuccess(const String &ssid, const String &host);
    void drawPingFail(const String &ssid, const String &host);

    void onError();
};
