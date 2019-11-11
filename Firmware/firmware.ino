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
  pic1,
  pic2,
  pic3,
  pic4
};

unsigned int updateCounter()
{
  unsigned int counter = 0;
  unsigned int newCounter = 0;
  unsigned int offset = 0;
  unsigned int newOffset = 0;
  const unsigned int OffsetAddress = 74;
  const unsigned int CounterAddress = 75;

  rst_info *resetInfo = ESP.getResetInfoPtr();
  if (resetInfo->reason != REASON_DEEP_SLEEP_AWAKE)
  {
    ESP.rtcUserMemoryWrite(OffsetAddress, &newOffset, sizeof(newOffset));
    
    for(int i = 0; i < 32; i++)
      ESP.rtcUserMemoryWrite(CounterAddress + i, &newCounter, sizeof(newCounter));

    return 0;
  }

  ESP.rtcUserMemoryRead(OffsetAddress, &offset, sizeof(offset));
  newOffset = offset;
    
  ESP.rtcUserMemoryRead(CounterAddress + offset, &counter, sizeof(counter));
  if (counter == 255)
  {
    counter = 0;
    newOffset = offset + 1;
    if (newOffset == 32)
    {
      ESP.rtcUserMemoryWrite(CounterAddress + offset, &counter, sizeof(counter));
      newOffset = 0;
    }
  }
  newCounter = counter + 1;
  if (newOffset != offset)
    ESP.rtcUserMemoryWrite(OffsetAddress, &newOffset, sizeof(newOffset));
    
  ESP.rtcUserMemoryWrite(CounterAddress + newOffset, &newCounter, sizeof(newCounter));

  return counter;
}

void setup() {
  
  display.init(115200);
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setFullWindow();
}

void loop() {
  unsigned int picNo = updateCounter() % 4;
 
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.drawInvertedBitmap(0, 0, pics[picNo], display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_BLACK);
  }
  while (display.nextPage());
  display.hibernate();

  ESP.deepSleep(45e6); // 45s deep sleep

}
