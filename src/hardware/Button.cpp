#include "Button.h"
#include <Arduino.h>

static constexpr unsigned long LONG_PRESS_MS = 600;
static constexpr unsigned long DEBOUNCE_DELAY_MS = 60;

Button::Button(uint8_t pin)
    : pin(pin),
      stableState(HIGH),
      lastStableState(HIGH),
      lastRawState(HIGH),
      pressedTime(0),
      lastDebounceTime(0),
      isPressing(false),
      longPressFired(false)
{
}

void Button::begin()
{
    pinMode(pin, INPUT_PULLUP);
}

ButtonEvent Button::update()
{
    ButtonEvent event = ButtonEvent::NONE;

    bool rawReading = digitalRead(pin);

    // ---- DEBOUNCE ----
    if (rawReading != lastRawState)
    {
        lastDebounceTime = millis();
        lastRawState = rawReading;
    }

    if (millis() - lastDebounceTime > DEBOUNCE_DELAY_MS)
    {
        stableState = rawReading;
    }

    // ---- PRESS START ----
    if (lastStableState == HIGH && stableState == LOW)
    {
        pressedTime = millis();
        isPressing = true;
        longPressFired = false;
    }

    // ---- LONG PRESS ----
    if (isPressing && !longPressFired &&
        millis() - pressedTime >= LONG_PRESS_MS)
    {
        longPressFired = true;
        event = ButtonEvent::LONG_PRESS;
    }

    // ---- RELEASE → SHORT PRESS ----
    if (lastStableState == LOW && stableState == HIGH)
    {
        if (isPressing && !longPressFired)
        {
            event = ButtonEvent::SHORT_PRESS;
        }
        isPressing = false;
    }

    lastStableState = stableState;
    return event;
}
