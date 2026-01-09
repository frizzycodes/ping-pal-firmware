const int BUTTON_PIN = 5;
const unsigned long LONG_PRESS_THRESHOLD = 600; // ms
const unsigned long DEBOUNCE_DELAY = 60; // ms

bool buttonState = HIGH;
bool lastButtonState = HIGH;

unsigned long pressedTime = 0;
bool isPressing = false;
bool isLongPress = false;
bool isShortPress = false;
bool longPressHandled = false;   // ensures long press fires once

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // use INPUT_PULLUP if wired to GND
  Serial.println("BUTTON test Started!");
}

void loop() {
  buttonState = digitalRead(BUTTON_PIN);

  // Reset flags each loop (they become "events")
  isShortPress = false;
  isLongPress  = false;

  // Detect Button Press
  if (lastButtonState == HIGH && buttonState == LOW) {
    pressedTime = millis();
    isPressing = true;
    longPressHandled = false;
  }

  // While holding → detect long press once
  if (isPressing && !longPressHandled) {
    if (millis() - pressedTime >= LONG_PRESS_THRESHOLD) {
      isLongPress = true;
      longPressHandled = true;
      Serial.println("Long Press!");
    }
  }

  // Detect Button Release
  if (lastButtonState == LOW && buttonState == HIGH) {
    unsigned long pressDuration = millis() - pressedTime;

    if (pressDuration > DEBOUNCE_DELAY && !longPressHandled) {
      isShortPress = true;
      Serial.println("Short Press!");
    }

    isPressing = false;
  }

  lastButtonState = buttonState;
}
