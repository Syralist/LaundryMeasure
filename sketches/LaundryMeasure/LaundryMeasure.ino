// For a display via I2C
#include <Wire.h>
#include "SSD1306.h"
#include <Adafruit_ADS1015.h>

#include <Time.h>
#include <Timezone.h>

// Include the UI lib
#include "OLEDDisplayUi.h"
// Include custom images
#include "images.h"

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#ifndef _BOT_ID
#include "telegram.h"
#endif


//Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone CE(CEST, CET);
TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev
time_t utc;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"de.pool.ntp.org");

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

unsigned int updateInterval = 10000;
unsigned int lastUpdate = 0;

unsigned int debounceInterval = 200;
unsigned int lastJoystick = 0;
bool triggeredJoystick = false;

enum Xpos {links, xmitte, rechts};
enum Ypos {oben, ymitte, unten};

bool WiFiConnected = false;

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, D1, D2);
OLEDDisplayUi ui( &display );

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_10);
    display->drawString(0, 0, String(timeClient.getFormattedTime()));
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setFont(ArialMT_Plain_16);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  if(!WiFiConnected)
      display->drawString(0, 20, "Connecting...");
  else
      display->drawString(0, 20, String(WiFi.SSID()));
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
    // Besides the default fonts there will be a program to convert TrueType fonts into this format
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_10);
    display->drawString(0 + x, 10 + y, "Arial 10");

    display->setFont(ArialMT_Plain_16);
    display->drawString(0 + x, 20 + y, "Arial 16");

    display->setFont(ArialMT_Plain_24);
    display->drawString(0 + x, 34 + y, "Arial 24");
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawFrame1, drawFrame2};

// how many frames are there?
int frameCount = 2;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  // put your setup code here, to run once:

  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(60);
  // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);
  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);
  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);
  // Add frames
  ui.setFrames(frames, frameCount);
  // Add overlays
  ui.setOverlays(overlays, overlaysCount);
  ui.setTimePerTransition(250);
  ui.disableAutoTransition();
  // Initialising the UI will init the display too.
  ui.init();
  display.flipScreenVertically();
  int remainingTimeBudget = ui.update();

  /* wifiMulti.addAP("Hackerspace", "makehackmodify"); */
  WiFiManager wifiManager;
  wifiManager.autoConnect("WaschmaschineAP");
  WiFiConnected = true;
  timeClient.begin();
  String telegram = "https://api.telegram.org/" + String(_BOT_ID) + "/sendMessage?chat_id=" + _CHAT_ID + "&text=" + timeClient.getFormattedTime();
  Serial.println(telegram);
  HTTPClient http;
  http.begin(telegram);
  int httpCode = http.GET();
  // httpCode will be negative on error
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();

  ads.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  if((millis()-lastUpdate)>updateInterval)
  {
    lastUpdate = millis();
    timeClient.update();
    if(CE.utcIsDST(timeClient.getEpochTime()))
    {
        timeClient.setTimeOffset(7200);
    }
    else
    {
        timeClient.setTimeOffset(3600);
    }
  }
  if(triggeredJoystick && ((millis()-lastJoystick)>debounceInterval))
  {
      triggeredJoystick = false;
  }
  if(!triggeredJoystick)
  {
      if(getXpos() == rechts)
          ui.nextFrame();
      else if(getXpos() == links)
          ui.previousFrame();
  }
  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
      // You can do some work here
      // Don't do stuff if you are below your
      // time budget.
      delay(remainingTimeBudget);
  }
}

int16_t getXaxis()
{
    return ads.readADC_SingleEnded(3);
}

int16_t getYaxis()
{
    return ads.readADC_SingleEnded(2);
}

Xpos getXpos()
{
    if(getXaxis() < 5000)
        return links;
    else if(getXaxis() > 13000)
        return rechts;
    else
        return xmitte;
}

Ypos getYpos()
{
    if(getYaxis() < 5000)
        return oben;
    else if(getYaxis() > 13000)
        return unten;
    else
        return ymitte;
}

