#include "OledDisplay.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "./fonts/FreeSerif9pt7b.h"
#include "./fonts/FreeSerifBold9pt7b.h"
#include "./hardware/OledAssets.h"

static Adafruit_SSD1306 display(128, 64, &Wire);

OledDisplay::OledDisplay()
    : bootFrameIndex(0),
      lastBootFrameTime(0),
      bootStartTime(0),
      bootAnimationDone(false)
{
}

/* =====================
   INIT
   ===================== */
void OledDisplay::begin()
{
    Wire.begin(21, 22);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    clear();
}

void OledDisplay::clear()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setTextWrap(false);
    display.display();
}

/* =====================
   BOOT
   ===================== */
void OledDisplay::onBootEnter()
{
    bootFrameIndex = 0;
    bootAnimationDone = false;
    lastBootFrameTime = millis();
    bootStartTime = millis();
    drawBootFrame(0);
}

void OledDisplay::updateBoot()
{
    if (bootAnimationDone)
        return;

    unsigned long now = millis();
    if (now - lastBootFrameTime >= BOOT_FRAME_INTERVAL_MS)
    {
        lastBootFrameTime = now;
        bootFrameIndex++;

        if (bootFrameIndex < BOOT_FRAME_COUNT)
            drawBootFrame(bootFrameIndex);
        else
            bootAnimationDone = true;
    }
}

bool OledDisplay::isBootDone() const
{
    return bootAnimationDone &&
           (millis() - bootStartTime >= BOOT_MIN_TIME_MS);
}

void OledDisplay::drawBootFrame(uint8_t frame)
{
    display.clearDisplay();
    display.setTextSize(1);

    switch (frame)
    {
    case 0:
        display.drawBitmap(52, 20, bootFrame1, 24, 24, 1);
        break;
    case 1:
        display.drawBitmap(52, 20, bootFrame2, 24, 24, 1);
        break;
    case 2:
        display.drawBitmap(52, 20, bootFrame3, 24, 24, 1);
        break;
    case 3:
        display.drawBitmap(52, 20, bootFrame4, 24, 24, 1);
        break;
    case 4:
        const char *startMsg = "Starting PINGPAL";
        int startMsgWidth = strlen(startMsg) * 6;
        int startMsgX = (128 - startMsgWidth) / 2;
        display.setCursor(startMsgX, 28);
        display.print(startMsg);
        break;
    }

    display.display();
}

void OledDisplay::drawWiFiConnecting()
{
    display.clearDisplay();
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setFont(&FreeSerif9pt7b);
    display.setCursor(23, 27);
    display.print("Connecting");

    display.setCursor(58, 44);
    display.print("Wifi");

    display.drawLine(1, 0, 127, 0, 1);

    // download
    display.drawBitmap(36, 32, image_wifi_1, 17, 16, 1);

    display.drawLine(0, 0, 0, 63, 1);

    display.drawLine(127, 0, 127, 63, 1);

    display.drawLine(1, 63, 127, 63, 1);

    display.display();
}

/* =====================
   SETUP CONFIRM
   ===================== */
void OledDisplay::drawSetupConfirmation()
{
    display.clearDisplay();
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setFont(&FreeSerifBold9pt7b);
    display.setCursor(47, 16);
    display.print("Enter");

    display.setCursor(13, 30);
    display.print("Setup Mode?");

    display.drawLine(1, 0, 127, 0, 1);

    display.setFont();
    display.setCursor(11, 38);
    display.print("Long Press");

    display.drawLine(0, 0, 0, 63, 1);

    display.setCursor(11, 49);
    display.print("Short Press");

    display.drawLine(127, 0, 127, 63, 1);

    display.drawBitmap(85, 40, image_menu_cursor, 3, 5, 1);

    display.drawLine(1, 63, 127, 63, 1);

    display.drawBitmap(85, 51, image_menu_cursor, 3, 5, 1);

    display.setCursor(99, 38);
    display.print("YES");

    display.setCursor(104, 49);
    display.print("NO");

    display.drawBitmap(30, 4, image_info, 15, 16, 1);

    display.display();
}

/* =====================
   SETUP MODE
   ===================== */
void OledDisplay::drawSetupMode(const String &ip)
{
    display.clearDisplay();
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setFont(&FreeSerif9pt7b);
    display.setCursor(8, 19);
    display.print("SETUP MODE");

    display.setFont();
    display.setCursor(20, 25);
    display.print("Open in Browser");

    display.setFont(&FreeSerifBold9pt7b);
    display.setCursor(34, 53);
    display.print(ip);

    display.drawLine(1, 0, 127, 0, 1);

    display.drawLine(127, 0, 127, 63, 1);

    display.drawLine(0, 0, 0, 63, 1);

    display.drawLine(1, 63, 127, 63, 1);

    display.drawBitmap(28, 46, image_menu_cursor, 3, 5, 1);

    display.drawBitmap(8, 40, image_earth_bits, 15, 16, 1);

    display.display();

    display.display();
}

/* =====================
   WIFI DISCONNECTED
   ===================== */
void OledDisplay::drawWiFiDisconnected()
{
    display.clearDisplay();
    updateDots();
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setFont(&FreeSerif9pt7b);
    display.setCursor(15, 34);
    display.print("Disconnected");

    display.setCursor(54, 22);
    display.print("Wifi");

    display.drawLine(1, 0, 127, 0, 1);

    display.drawBitmap(35, 9, image_wifi_1, 17, 16, 1);

    display.drawLine(0, 0, 0, 63, 1);

    display.setFont();
    display.setCursor(32, 40);
    display.print("Retrying");

    display.drawLine(127, 0, 127, 63, 1);

    display.drawLine(1, 63, 127, 63, 1);

    display.display();

    drawDots(80, 40);
    display.display();
}

/* =====================
   ONLINE PINGING
   ===================== */
void OledDisplay::drawOnlinePinging(
    const String &ssid,
    const String &host)
{
    display.clearDisplay();
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setFont(&FreeSerif9pt7b);
    display.setCursor(30, 17);
    display.print("Pinging...");

    display.drawLine(1, 0, 127, 0, 1);

    display.drawBitmap(42, 37, image_menu_cursor, 3, 5, 1);

    display.drawLine(0, 0, 0, 63, 1);

    display.setFont(&FreeSerifBold9pt7b);
    display.setCursor(51, 44);
    display.print(host);

    display.drawLine(127, 0, 127, 63, 1);

    display.drawLine(1, 63, 127, 63, 1);

    display.drawBitmap(21, 31, image_wifi_3, 15, 16, 1);

    display.display();
}

/* =====================
   PING SUCCESS
   ===================== */
void OledDisplay::drawPingSuccess(
    const String &ssid,
    const unsigned int &latency,
    const unsigned int &checkTime)
{
    display.clearDisplay();
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setFont(&FreeSerif9pt7b);
    display.setCursor(32, 17);
    String displaySsid = ssid.substring(0, 10) + "...";
    display.print(displaySsid);

    display.setCursor(31, 35);
    display.print("ONLINE");

    display.drawBitmap(4, 4, image_wifi_2, 19, 16, 1);

    display.setFont();
    display.setCursor(6, 51);
    display.print("Last Check");

    display.setCursor(6, 40);
    display.print("Latency");

    display.drawLine(1, 0, 127, 0, 1);

    display.drawLine(0, 0, 0, 63, 1);

    display.drawLine(127, 0, 127, 63, 1);

    display.drawLine(1, 63, 127, 63, 1);

    display.drawBitmap(25, 27, image_menu_cursor, 3, 5, 1);

    display.drawBitmap(25, 10, image_menu_cursor, 3, 5, 1);

    display.drawBitmap(69, 53, image_menu_cursor, 3, 5, 1);

    display.drawBitmap(53, 42, image_menu_cursor, 3, 5, 1);

    display.setCursor(64, 40);
    display.print(latency);
    display.print("ms");

    display.setCursor(77, 51);
    display.print(checkTime);
    display.print("s ago");

    display.drawBitmap(5, 22, image_range_bar, 15, 16, 1);

    display.display();
}

/* =====================
   PING FAIL
   ===================== */
void OledDisplay::drawPingFail(
    const String &ssid,
    const unsigned int &checkTime)
{
    display.clearDisplay();
    String displaySsid = ssid.substring(0, 10) + "...";
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setFont(&FreeSerif9pt7b);
    display.setCursor(32, 17);
    display.print(displaySsid);

    display.setCursor(31, 34);
    display.print("OFFLINE");

    display.drawBitmap(4, 4, image_wifi_2, 19, 16, 1);

    display.setFont();
    display.setCursor(6, 51);
    display.print("Last Check");

    display.setCursor(6, 40);
    display.print("Latency");

    display.drawLine(1, 0, 127, 0, 1);

    display.drawLine(0, 0, 0, 63, 1);

    display.drawLine(127, 0, 127, 63, 1);

    display.drawLine(1, 63, 127, 63, 1);

    display.drawBitmap(25, 26, image_menu_cursor, 3, 5, 1);

    display.drawBitmap(25, 10, image_menu_cursor, 3, 5, 1);

    display.drawBitmap(69, 53, image_menu_cursor, 3, 5, 1);

    display.drawBitmap(53, 42, image_menu_cursor, 3, 5, 1);

    display.setCursor(60, 40);
    display.print("No Response");

    display.setCursor(77, 51);
    display.print(checkTime);
    display.print("s ago");

    display.drawBitmap(6, 22, image_no_range_bar, 15, 16, 1);

    display.display();
}

void OledDisplay::updateDots()
{
    unsigned long now = millis();

    if (now - lastDotTime >= DOT_INTERVAL_MS)
    {
        lastDotTime = now;

        if (dotCount < MAX_DOTS)
            dotCount++;
        else
            dotCount = 0;
    }
}
void OledDisplay::drawDots(int x, int y)
{
    display.setCursor(x, y);

    for (uint8_t i = 0; i < dotCount; i++)
        display.print(".");
}

void OledDisplay::onError()
{
    display.clearDisplay();
    display.drawBitmap(12, 16, errorSignal, 30, 32, 1);
    display.setTextColor(1);
    display.setTextSize(2);
    display.setTextWrap(false);
    display.setCursor(50, 25);
    display.print("ERROR");

    display.display();
}