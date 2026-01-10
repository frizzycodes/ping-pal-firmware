#include "Led.h"
#include <Arduino.h>

Led::Led(uint8_t rPin, uint8_t gPin, uint8_t bPin)
    : rPin(rPin),
      gPin(gPin),
      bPin(bPin)
{
}

void Led::begin()
{
    pinMode(rPin, OUTPUT);
    pinMode(gPin, OUTPUT);
    pinMode(bPin, OUTPUT);
    setColor(LedColor::OFF);
}

void Led::setColor(LedColor color)
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
