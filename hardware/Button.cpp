#include "Button.h"
#include

// Internal Button Specific tuning (private to this file)
static constexpr unsigned long LONG_PRESS_MS = 600;
static constexpr unsigned long DEBOUNCE_DELAY_MS = 60;

bool buttonState = HIGH;
bool lastButtonState = HIGH;

Button::Button(uint8_t pin)
    : ping(ping),
      buttonState()

          unsigned long pressedTime = 0;
unsigned long lastDebounceTime = 0;

bool isPressing = false;
bool longPressFired = false;

void setup()
{
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    Serial.println("BUTTON test Started!");
}

void loop()
{
    bool reading = digitalRead(BUTTON_PIN);

    // Debounce (edge-agnostic)
    if (reading != lastButtonState)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
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
        if (millis() - pressedTime >= LONG_PRESS_THRESHOLD)
        {
            longPressFired = true;
            Serial.println("Long Press!");
        }
    }

    // -------- RELEASE DETECT --------
    if (lastButtonState == LOW && buttonState == HIGH)
    {
        if (isPressing && !longPressFired)
        {
            Serial.println("Short Press!");
        }

        isPressing = false;
    }

    lastButtonState = buttonState;
}
