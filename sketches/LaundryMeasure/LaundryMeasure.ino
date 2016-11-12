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

#define MAX_BUFFER_SIZE 2500

#ifndef _BOT_ID
#include "telegram.h"
#endif

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

enum Xpos {links, xmitte, rechts};
enum Ypos {oben, ymitte, unten};


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

unsigned int rmsInterval = 5000;
unsigned int rmsLastUpdate = 0;
double rmsValue = 0.0;

unsigned int debounceInterval = 200;
unsigned int lastJoystick = 0;
bool triggeredJoystick = false;

bool WiFiConnected = false;
bool NTPupdated = false;
bool MessageSentBooted = false;

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, D1, D2);
OLEDDisplayUi ui( &display );

#define OFFSET 8820
#define SCALE 1.65/OFFSET
#define VTOA 16.0/1.65

double Volts(int digits)
{
    return (double)(digits - OFFSET) * SCALE;
    /* return (double)(digits) * SCALE; */
}

double Irms(int channel)
{
    double sumSquares;
    double value;
    unsigned int samples = 0;
    unsigned int millisStart = millis();
    while( (millis()-millisStart) < 200 )
    {
        value = Volts(ads.readADC_SingleEnded(channel));
        sumSquares += value * value;
        samples++;
    }
    return VTOA * sqrt(sumSquares / (double)samples);
}

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_10);
    display->drawString(0, 0, timeClient.getFormattedTime());
    display->drawString(display->getStringWidth(timeClient.getFormattedTime())+1, 0, WiFi.SSID());
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    display->setFont(ArialMT_Plain_16);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    if(!WiFiConnected)
        display->drawString(0, 20, "Connecting...");
    else
        display->drawStringMaxWidth(0, 20, 128, String("Herzlich Willkommen!"));
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
    // Besides the default fonts there will be a program to convert TrueType fonts into this format
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    /* display->setFont(ArialMT_Plain_10); */
    /* display->drawString(0 + x, 10 + y, "Arial 10"); */

    display->setFont(ArialMT_Plain_16);
    display->drawString(0 + x, 17 + y, "Digits: " + String( ads.readADC_SingleEnded(1) ) );
    display->drawString(0 + x, 28 + y, "Volts:  " + String( Volts(ads.readADC_SingleEnded(1)) ) );
    if( (millis() - rmsLastUpdate) > rmsInterval )
    {
        rmsValue = Irms(1);
        Serial.printf("%d Irms: %f\n", millis(), rmsValue);
        rmsLastUpdate = millis();
    }
    display->drawString(0 + x, 39 + y, "Amps:   " + String( rmsValue ) );

    /* display->setFont(ArialMT_Plain_24); */
    /* display->drawString(0 + x, 34 + y, "Arial 24"); */
}

String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
        if (c == ' '){
            encodedString+= '+';
        } else if (isalnum(c)){
            encodedString+=c;
        } else{
            code1=(c & 0xf)+'0';
            if ((c & 0xf) >9){
                code1=(c & 0xf) - 10 + 'A';
            }
            c=(c>>4)&0xf;
            code0=c+'0';
            if (c > 9){
                code0=c - 10 + 'A';
            }
            code2='\0';
            encodedString+='%';
            encodedString+=code0;
            encodedString+=code1;
            //encodedString+=code2;
        }
        yield();
    }
    return encodedString;
}

void sendTelegram(String message)
{
    String telegram = "http://steffiundthomas.net/telegramrelais.php?bot_token=" + String(_BOT_ID) + "&chat_id=" + _CHAT_ID + "&body=" + urlencode(message);
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

    WiFiManager wifiManager;
    wifiManager.autoConnect("WaschmaschineAP");
    WiFiConnected = true;
    timeClient.begin();

    ads.begin();
}

void loop() {
    // put your main code here, to run repeatedly:

    if((millis()-lastUpdate)>updateInterval)
    {
        lastUpdate = millis();
        NTPupdated = timeClient.update();
        if(CE.utcIsDST(timeClient.getEpochTime()))
        {
            timeClient.setTimeOffset(7200);
        }
        else
        {
            timeClient.setTimeOffset(3600);
        }
    }
    if( !MessageSentBooted && NTPupdated )
    {
        sendTelegram("Gebootet um: " + timeClient.getFormattedTime()); 
        MessageSentBooted = true;
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

