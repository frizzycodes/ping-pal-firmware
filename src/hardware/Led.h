#pragma once
#include <stdint.h>

enum class LedColor : uint8_t
{
    OFF,
    RED,
    BLUE,
    GREEN,
    YELLOW
};

enum class LedPattern : uint8_t
{
    SOLID,
    BLINK_SLOW,
    BLINK_FAST
};

class Led
{
private:
    uint8_t rPin;
    uint8_t gPin;
    uint8_t bPin;

    LedColor currentColor;
    LedPattern currentPattern;
    unsigned long lastToggleTime;
    bool isOn;

public:
    Led(uint8_t rPin, uint8_t gPin, uint8_t bPin);
    void begin();
    void setColor(LedColor color, LedPattern pattern);
    void update();
    void set(LedColor color, LedPattern pattern);
};