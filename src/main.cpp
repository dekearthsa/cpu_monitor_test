#include <Arduino.h>
#include "SH1106Wire.h"

const int OLED_SDA = 21;   // ESP32 pin D21 -> OLED SDA
const int OLED_SCL = 22;   // ESP32 pin D22 -> OLED SCL
const int OLED_ADDR = 0x3C;

SH1106Wire display(OLED_ADDR, OLED_SDA, OLED_SCL, GEOMETRY_128_64, I2C_ONE, 400000);

void drawScreen() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);

  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 8, "Hello ESP32");

  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 32, "OLED I2C OK");
  display.drawString(64, 48, "SDA D21  SCL D22");

  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(300);

  display.init();
  display.flipScreenVertically();
  drawScreen();

  Serial.println("OLED ready");
}

void loop() {
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 8, "Hello ESP32");

    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 32, "OLED I2C OK");
    display.drawString(64, 48, "Uptime " + String(millis() / 1000) + "s");
    display.display();
  }
}
