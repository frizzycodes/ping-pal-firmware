#include "Button.h"
#include <Arduino.h>

// Internal Button Specific tuning (private to this file)
static constexpr unsigned long LONG_PRESS_MS = 600;
static constexpr unsigned long DEBOUNCE_DELAY_MS = 60;

bool buttonState = HIGH;
bool lastButtonState = HIGH;

Button::Button(uint8_t pin)
    : pin(pin),
      buttonState(HIGH),
      lastButtonState(false),
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

    bool reading = digitalRead(pin);

    // Debounce (eboth edges)
    if (reading != lastButtonState)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY_MS)
    {
        buttonState = reading;
    }

    // -------- PRESS DETECT --------
    if (lastButtonState == HIGH && buttonState == LOW)
    {
        pressedTime = millis();
        isPressing = true;
        longPressFired = false;
    }

    // -------- LONG PRESS --------
    if (isPressing && !longPressFired)
    {
        if (millis() - pressedTime >= LONG_PRESS_MS)
        {
            longPressFired = true;
            // Serial.println("Long Press!");
            return ButtonEvent::LONG_PRESS;
        }
    }

    // -------- RELEASE DETECT --------
    if (lastButtonState == LOW && buttonState == HIGH)
    {
        if (isPressing && !longPressFired)
        {
            // Serial.println("Short Press!");
            return ButtonEvent::SHORT_PRESS;
        }

        isPressing = false;
    }

    lastButtonState = buttonState;
}