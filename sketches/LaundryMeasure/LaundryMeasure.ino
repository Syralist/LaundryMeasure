// For a display via I2C
#include <Wire.h>
#include "SSD1306.h"
#include <Adafruit_ADS1015.h>

// Include custom images
#include "images.h"

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>

ESP8266WiFiMulti wifiMulti;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, 7200);

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, D1, D2);

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

unsigned int updateInterval = 10000;
unsigned int lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  // put your setup code here, to run once:

  wifiMulti.addAP("Hackerspace", "makehackmodify");
  timeClient.begin();

  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  ads.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  int wifiStatus = wifiMulti.run();
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, String(millis()));

  if((millis()-lastUpdate)>updateInterval && wifiStatus == WL_CONNECTED)
  {
    lastUpdate = millis();
    timeClient.update();
  }
  if(wifiStatus == WL_CONNECTED)
  {
    display.drawString(0, 20, String(timeClient.getFormattedTime()));
    int16_t adc0, adc1, adc2, adc3;
    adc0 = ads.readADC_SingleEnded(0);
    adc1 = ads.readADC_SingleEnded(1);
    adc2 = ads.readADC_SingleEnded(2);
    adc3 = ads.readADC_SingleEnded(3);
    display.drawString(0, 30, String(adc2));
    display.drawString(0, 40, String(adc3));
  }
  else
  {
    display.drawString(0, 20, "Connecting...");
  }
  
  // write the buffer to the display
  display.display();
  delay(10);
}

