#pragma once
#include <stdint.h>

enum class ButtonEvent : uint8_t
{
    NONE,
    SHORT_PRESS,
    LONG_PRESS
};

class Button
{
public:
    explicit Button(uint8_t pin);

    void begin();
    ButtonEvent update();

private:
    uint8_t pin;

    bool stableState;
    bool lastStableState;
    bool lastRawState;

    unsigned long pressedTime;
    unsigned long lastDebounceTime;

    bool isPressing;
    bool longPressFired;
};