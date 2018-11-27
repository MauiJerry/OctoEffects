/*
   OctoEfx_testDazzle
*/

#include <OctoWS2811.h>
#include "OctoEffects.h"

//////////////////////////////////////////

// Comment out to turn off Serial and LED debugging indicators for better performance
#define DEBUG_MIN
#define DEBUG_STATE
//#define DEBUG_LED
//////////////////////////////////////////


#define STRIP_SIZE 50
#define INTENSITY 255      //for NeoPixels

//////////////////////////////////////////

const int WIN_START = 0;
const int numWindows = 1;

int defaultEfxCount = 100;

//////////////////////////////////////////

const int memSize = STRIP_SIZE *6;
DMAMEM int displayMemory[memSize];
int drawingMemory[memSize];

const int config = WS2811_GRB | WS2811_800kHz;

OctoStrip octo = OctoStrip(STRIP_SIZE, displayMemory, drawingMemory, config);

OctoWindow window = OctoWindow(0, STRIP_SIZE);
//////////////////////////////////////////

const uint32_t aNicePurple = octo.Color(128, 0, 50);

int StartWinState = 0;// negative one to hold initial Black state
int windowState = StartWinState;
int maxEfx = 255;

//////////////////////////////////////////
void setup() {
  // use serial line for debugging output
  Serial.begin(115200);
  delay(500); // delay a bit when we start so we can open arduino serial monitor window

  Serial.println("--Starting BM2017 TestMultiSparkle with BG color --");

  // start the strip.  do this first for all strips
  octo.begin();

  blinkWholeStrip();

  // initially use built in efx, later update to new efx
  // tip efx will quick flash its id number somehow
  //window.setRainEffect( numRainDrops, rainWaitTime, defaultEfxCount);
  window.fillBlack( );
  window.setHoldEfx(1000);
  // change to setTipEfx

  Serial.println("Begin Loop ... cur time: "); Serial.println(millis());
}

void loop() {

  // grab the current time in class method
  OctoWindow::updateTime();
  int delayTime = 250;
  int effectCount = 10; // 10 seconds
  // window state machine
  if (window.effectDone())
  {
    windowState++;
    if (windowState > maxEfx) {
      Serial.println("wrap around windowState");
      windowState = 0;
    }

    window.setDazzleEfx( 500, 50, 100);

  }
  window.updateWindow();

  octo.show();

}

//////////////////////////////////////////
void blinkWholeStrip(void)
{
  octo.clearStrip();
  octo.show();
  delay(1000);
  octo.fillStrip(aNicePurple);
  octo.show();
  delay(1000);
  octo.clearStrip();
  octo.show();
  delay(1000);
}
