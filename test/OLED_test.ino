#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET
);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(21,22);
  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)){
    Serial.println("OLED Not Found!");
    while(true){
      //do nothing infinite loop
    }
  }
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  display.println("Hello");
  display.println("OLED works Fine!");
  display.display();
  display.println("newLine");
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:

}
