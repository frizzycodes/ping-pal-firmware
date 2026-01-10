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

class Led
{
private:
    uint8_t rPin;
    uint8_t gPin;
    uint8_t bPin;

public:
    Led(uint8_t rPin, uint8_t gPin, uint8_t bPin);
    void begin();
    void setColor(LedColor color);
};