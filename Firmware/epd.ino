#include <ESP8266WiFi.h>
#include <GxEPD2.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_EPD.h>
#include <GxEPD2_GFX.h>

#include <Fonts/FreeMonoBold9pt7b.h>
#include "pic.h"

GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=D8*/ D3, /*DC=D3*/ D4, /*RST=D4*/ D1, /*BUSY=D2*/ D2));
const unsigned char* pics[] = {
  pic,
  pic2
};

unsigned int updateCounter()
{
  unsigned int counter = 0;
  unsigned int newCounter;
  
  ESP.rtcUserMemoryRead(64, &counter, sizeof(counter));
  if (counter >= 2)
    counter = 0;
  newCounter = counter + 1;
  ESP.rtcUserMemoryWrite(64, &newCounter, sizeof(counter));
  return counter;
}

void setup() {
  
  display.init(115200);
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setFullWindow();
}

void loop() {
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.drawInvertedBitmap(0, 0, pics[updateCounter()], display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_BLACK);
  }
  while (display.nextPage());
  display.hibernate();

  ESP.deepSleep(60e6); // 60s deep sleep

}
