#include "Led.h"
#include <Arduino.h>

Led::Led(uint8_t rPin, uint8_t gPin, uint8_t bPin)
    : rPin(rPin),
      gPin(gPin),
      bPin(bPin),
      currentColor(LedColor::OFF),
      currentPattern(LedPattern::SOLID),
      lastToggleTime(0),
      isOn(false)
{
}

void Led::begin()
{
    pinMode(rPin, OUTPUT);
    pinMode(gPin, OUTPUT);
    pinMode(bPin, OUTPUT);
    setColor(LedColor::OFF, LedPattern::SOLID);
}
void Led::set(LedColor color, LedPattern pattern)
{
    currentColor = color;
    currentPattern = pattern;
    lastToggleTime = millis();
    isOn = true;
    setColor(color, pattern);
}
void Led::setColor(LedColor color, LedPattern pattern)
{
    digitalWrite(rPin, LOW);
    digitalWrite(gPin, LOW);
    digitalWrite(bPin, LOW);

    switch (color)
    {
    case LedColor::RED:
        digitalWrite(rPin, HIGH);
        digitalWrite(gPin, LOW);
        digitalWrite(bPin, LOW);
        break;
    case LedColor::GREEN:
        digitalWrite(rPin, LOW);
        digitalWrite(gPin, HIGH);
        digitalWrite(bPin, LOW);
        break;
    case LedColor::BLUE:
        digitalWrite(rPin, LOW);
        digitalWrite(gPin, LOW);
        digitalWrite(bPin, HIGH);
        break;
    case LedColor::YELLOW:
        digitalWrite(rPin, HIGH);
        digitalWrite(gPin, HIGH);
        digitalWrite(bPin, LOW);
        break;

    case LedColor::OFF:
    default:
        break;
    }
}

void Led::update()
{
    if (currentPattern == LedPattern::SOLID)
        return;

    unsigned long now = millis();
    unsigned long intervel = (currentPattern == LedPattern::BLINK_SLOW) ? 800 : 200;

    if (now - lastToggleTime >= intervel)
    {
        lastToggleTime = now;
        isOn = !isOn;
        if (isOn)
        {
            setColor(currentColor, currentPattern);
        }
        else
        {
            setColor(LedColor::OFF, LedPattern::SOLID);
        }
    }
}
