/* 
 * OctoEfx_Test- by Jerry Isdale
 * OctoEffects is rewriting of NeoEffects to use PJRC OctoWS2811 instead of Adafruit NeoPixel
 *  NeoEffects is available at https://github.com/MauiJerry/NeoEffects_library
 *  this is a test tool steps thru each of the internally defined effects

 Required Connections
  --------------------
    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
    pin 14: LED strip #2    All 8 are the same length.
    pin 7:  LED strip #3
    pin 8:  LED strip #4    A 100 ohm resistor should used
    pin 6:  LED strip #5    between each Teensy pin and the
    pin 20: LED strip #6    wire to the LED strip, to minimize
    pin 21: LED strip #7    high frequency ringining & noise.
    pin 5:  LED strip #8
    pin 15 & 16 - Connect together, but do not use
    pin 4 - Do not use
    pin 3 - Do not use as PWM.  Normal use is ok.

 
 */

#include <OctoWS2811.h>
#include "OctoEffects.h"

//////////////////////////////////////////
#define INTENSITY 255      //for NeoPixels
#define STRIP_SIZE 20
// call it 8 strips so we can test each
#define NUMSTRIPS 8

#define WINDOW_SIZE 40
#define STRIP_1_PIN 13
#define DEFAULT_DELAYTIME 100

uint32_t
BLANK = OctoStrip::Color(0, 0, 0),
BLUE = OctoStrip::Color(0, 0, INTENSITY),
WHITE = OctoStrip::Color(INTENSITY, INTENSITY, INTENSITY),
RED = OctoStrip::Color(INTENSITY, 0, 0),
GREEN = OctoStrip::Color(0, INTENSITY, 0),
YELLOW = OctoStrip::Color(INTENSITY, INTENSITY, 0),
BROWN = OctoStrip::Color(165, 42, 0),
ORANGE = OctoStrip::Color(INTENSITY, 128, 0);

const int maxBrightness =255;

int defaultEfxCount = 10;

const int memSize = STRIP_SIZE *6;
DMAMEM int displayMemory[memSize];
int drawingMemory[memSize];

const int config = WS2811_GRB | WS2811_800kHz;

OctoStrip octo = OctoStrip(STRIP_SIZE, displayMemory, drawingMemory, config);

// a window on first 10 pix of each row
OctoWindow win0 = OctoWindow(octo.getAbsolutePixel(0,0), 10);
OctoWindow win1 = OctoWindow(octo.getAbsolutePixel(1,0), 10);
OctoWindow win2 = OctoWindow(octo.getAbsolutePixel(2,0), 10);
OctoWindow win3 = OctoWindow(octo.getAbsolutePixel(3,0), 10);
OctoWindow win4 = OctoWindow(octo.getAbsolutePixel(4,0), 10);
OctoWindow win5 = OctoWindow(octo.getAbsolutePixel(5,0), 10);
OctoWindow win6 = OctoWindow(octo.getAbsolutePixel(6,0), 10);
OctoWindow win7 = OctoWindow(octo.getAbsolutePixel(7,0), 10);

const uint32_t aNicePurple = octo.color(128, 0, 50);

int StartWinState = 1; // a hack to skip N steps when testing
int windowState = StartWinState;

//////////////////////////////////////////
void setup() {

   // use serial line for debugging output
  Serial.begin(115200);
  delay(500); // delay a bit when we start so we can open arduino serial monitor window
  
  Serial.println("Starting OctoEffects TestAll");

  // start the strip.  do this first for all strips
  octo.begin();
  octo.show();

  // NeoPixels can be very bright, and at full power can use lots of power
  // longer 'strips' require extra power to run full bright. 
  // brightness runs 0-255 and scales all colors to match that dark->bright
  octo.setBrightness(maxBrightness/2); 
  OctoWindow::updateTime();

  Serial.printf("win 0 window id = %d\n", win0.getId());
  Serial.printf("win 1 window id = %d\n", win1.getId());
  Serial.printf("win 2 window id = %d\n", win2.getId());
  Serial.printf("win 3 window id = %d\n", win3.getId());
  Serial.printf("win 4 window id = %d\n", win4.getId());
  Serial.printf("win 5 window id = %d\n", win5.getId());
  Serial.printf("win 6 window id = %d\n", win6.getId());
  Serial.printf("win 7 window id = %d\n", win7.getId());

  blinkWholeStrip();

  setSolidColorEfx();
  
  Serial.println("Update End of Setup");

  Serial.println("Begin Loop");
}

void setSolidColorEfx()
{
  Serial.println("set wins to solid colors");
//  // initially use built in efx, later update to new efx
  Serial.println("Set Red");
  win0.setSolidColorEfx(RED, 1000);
    Serial.println("UpdateWindow"); 
    win0.updateWindow();
  Serial.println("Set Green");
  win1.setSolidColorEfx(GREEN, 1000); win1.updateWindow();
  Serial.println("Set Blue");
  win2.setSolidColorEfx(BLUE, 1000);
  Serial.println("UpdateWindow"); 
  win2.updateWindow();
  win3.setSolidColorEfx(0xFF00FF, 1000); win3.updateWindow();
  win4.setSolidColorEfx(0xFFFF00, 1000); win4.updateWindow();
  win5.setSolidColorEfx(0x00FFFF, 1000); win5.updateWindow();
  win6.setSolidColorEfx(octo.Color(128, 0, 50), 1000); win6.updateWindow();
  win7.setSolidColorEfx(0xFFFFFF, 1000); win7.updateWindow();
   Serial.println("Show Octo");
   
  octo.show();

}

// this is BS actually.  with only one 'windowState' we really arent doing this on each randomly
void windowStateMachine(OctoWindow* window)
{
  int effectCount = defaultEfxCount; // changes per effect
  int delayTime = DEFAULT_DELAYTIME; // default is 1/4sec each step

  // window state machine
  if (window->effectDone()) {
    windowState++;
    Serial.printf("window %d state: %d \n  ", window->getId(),windowState);
    switch (windowState) {
      //1 = SolidColorEfx
      // 2 = circle fwd
      // 3 = circle rev
      // 4 = wipe
      // 5 = randomWipe
      // 6 = reverseWipe
      // 7 = Blink
      // 8 = sparkle
      // 9 = MultiSparkle
      // 10 = Fade Once
      // 11 = Fade Cycle
      // 12 = Fade Jump
      // 13 = rainbow 255 len
      // 14 = rainbow even distrb
      // 15 = ?
      case 1:
        Serial.println(" Solid BLUE hold 5 sec");
        delayTime = 5000;
        window->setSolidColorEfx(BLUE, delayTime);
        break;
      case 2:
      {
        delayTime = 100;
        effectCount = 3;
        int tmpcolor = octo.randomColor();
        Serial.print (" Circle fwd ");Serial.println(tmpcolor);
        window->setCircleEfx(tmpcolor, delayTime, effectCount,0);
        break;
      }
      case 3:
        Serial.println(" Circle rev");
        delayTime = 100;
        effectCount = 3;
        window->setCircleEfx(octo.randomColor(), delayTime, effectCount, 1);
        break;
     case 4:
        Serial.println(" WipeEfx");
        window->setWipeEfx(aNicePurple, delayTime );
        break;
     case 5:
        Serial.println(" randomWipeEfx");
        window->setRandomWipeEfx(octo.randomColor(), octo.randomColor(), delayTime );
        break;
     case 6:
        Serial.println(" reverseWipe");
        window->setReverseWipeEfx(octo.randomColor(), delayTime );
        break;
     case 7:
        Serial.println(" Blink");
        effectCount = 10;
        window->setBlinkEfx(aNicePurple, delayTime, effectCount);
        break;
     case 8:
        Serial.println(" Sparkle");
        effectCount = 20;
        window->setSparkleEfx(BLUE, delayTime, delayTime/2, effectCount);
        break;
     case 9:
        Serial.println(" MultiSparkle");
        effectCount = 20;
        window->setMultiSparkleEfx(aNicePurple, delayTime/2, delayTime, WINDOW_SIZE/5, effectCount);
        break;
    case 10:
        Serial.println(" Fade Once");
        window->setFadeEfx(0, octo.randomColor(), 10, window->fadeTypeOnce); // fade between two colors
        break;
    case 11:
        Serial.println(" Fade Cycle");
        effectCount = 2; // default = once
        window->setFadeEfx(0, octo.randomColor(), 10, window->fadeTypeCycle, effectCount); // fade between two colors
        break;
    case 12:
        Serial.println(" Fade JumpBack");
        effectCount = 2; // default = once
        window->setFadeEfx(0, octo.randomColor(), 10, window->fadeTypeJumpBack, effectCount); // fade between two colors
        break;
    case 13:
        Serial.println(" Rainbow 255 len");
        window->setRainbowEfx(delayTime, 0);
        break;
    case 14:
        Serial.println(" Rainbow even distrib");
        Serial.println(" Rainbow 255 len");
        window->setRainbowEfx(delayTime, 1);
        break;
    case 15:
        Serial.println(" Dazzle Effect");
        window->setDazzleEfx( 500, 50, 100);
        break;
    default: // black for sanity
      // when windowState increments past highest case above, it falls into state = 0
        Serial.println(" DEFAULT = black hold 5sec");
        windowState = StartWinState;
        window->fillBlack( );
        window->setHoldEfx(5000);
    }
  } else {
    Serial.printf("notDone window %d state: %d effectDone %d \n", window->getId(),windowState, window->effectDone());
  }
  
  // now update each Window - does one 'frame' of effect on the window
  Serial.println("windowStateMachine updateWindow");
  window->updateWindow();
  Serial.printf("windowStateMachine done win.efxDone: %d\n",window->effectDone());
}

void blinkWholeStrip(void)
{
  Serial.println("Start blinkWholeStrip");
  octo.clearStrip();
  octo.show();
  Serial.println("should be black");
  delayMicroseconds(100000); 
  Serial.println("Fill with Purple");   
  octo.fillStrip(aNicePurple);
  octo.show();
  Serial.println("should be purple");
  delayMicroseconds(500000); // hold 5 sec
  //delay(1000); // hold 1 sec
  octo.clearStrip();
  octo.show();
    Serial.println("should be black");
  delayMicroseconds(100000);
  Serial.println("End blinkWholeStrip");
}

void loop() {
  // grab the current time in class method
  OctoWindow::updateTime();
  
  windowStateMachine(&win0);
  
  octo.show();
  delayMicroseconds(100); 

}
