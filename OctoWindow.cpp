// OctoWindow
/** @file OctoWindow.cpp
 @brief Implements the OctoWindow class of NeoEfx library - providing concurrent effects on NeoPixels
 @author jerry isdale
 Windows into OctoWS8211 are based on Row which is defined by the pin number
 // rows correspond to PJRC pins
 // row  pin
 // 0    2
 // 1    14
 // 2    7
 // 3    8
 // 4    6
 // 5    20
 // 6    21
 // 7    5
 */

//#include "OctoStrip.h"
#include "OctoWindow.h"

//////
// to invoke the member function pointer...
//#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

// Class Variable definitions:

uint32_t OctoWindow::currTime = 0; // current Millis
int OctoWindow::s_nIDGenerator = 1;

/////////////////
// Constructor

OctoWindow::OctoWindow(int startPixel, int len)
{
    myStrip = OctoStrip::getInstance();
    myId = s_nIDGenerator++;
    myStartPixel = startPixel;
    myPixelCount = len;
    myEndPixel = myStartPixel + myPixelCount-1;
    myBgColor = 0; // default is black = 0;
    
    setNoEfx(); // this does rest of setup
}

void OctoWindow::printId(void)
{
    Serial.print("OctoWindow ");Serial.print(myId); Serial.print(" ");
}

void OctoWindow::printData(void)
{
    printId();
    Serial.print("   myStartPixel: ");Serial.println(myStartPixel);
    Serial.print("   myPixelCount: ");Serial.println(myPixelCount);
    Serial.print("   myEndPixel: ");Serial.println(myEndPixel);
    Serial.print("   effectCount: ");Serial.println(effectCount);
    Serial.print("   effectMaxCount: ");Serial.println(effectMaxCount);
    Serial.print("   efxDone: ");Serial.println(efxDone);
    Serial.print("   lastTime: ");Serial.println(lastTime);
    Serial.print("   effectDelay: ");Serial.println(effectDelay);
    //  Serial.print("   updateFunc: ");Serial.println((void *)curUpdateFunc);
    
}

void OctoWindow::updateWindow(void)
{
    if (!curUpdateFunc) {
            printId(); Serial.println("    No updateFunc");
        return; // no effect defined. quick return
    }
    
    // determine if the current Effect time has passed
    Serial.printf("updateWindow %d currT:%ld lastT: %ld efxDelay: %ld \n",getId(), OctoWindow::currTime, lastTime, effectDelay );
//    printId();
//    Serial.print("   curr time:");Serial.print(OctoWindow::currTime);
//    Serial.print("   lastTime: ");Serial.println(lastTime);
//    Serial.print("   effectDelay: ");Serial.println(effectDelay);
   
    if (OctoWindow::currTime - lastTime < effectDelay)
    {
        printId(); Serial.println("    Not yet time");
       return;
    }
    Serial.println("    Effect Delay Done, invoke update");
    
    // invoke current effect function update
    (this->*curUpdateFunc)();
    
    myStrip->setStripChanged(); // mark the strip changed
    lastTime = OctoWindow::currTime; // NOW update the time
}

void OctoWindow::fillColor(uint32_t color)
{
//    Serial.print("fill color ");Serial.print(color);
//    Serial.print(" myStarPixel "); Serial.print(myStartPixel);
//    Serial.print(" myEndPixel "); Serial.print(myEndPixel);
    for (int i=myStartPixel; i <= myEndPixel; i++)
    {
//        Serial.print("fill pix ");Serial.println(i);
       myStrip->setPixelColor(i, color); //set rest of window to black
    }
//    Serial.println("done fill, setStripChanged");
    myStrip->setStripChanged(); // mark the strip changed
    
//    Serial.println("fill color done");
}

void OctoWindow::fillBlack()
{
//    Serial.println("---fill Black")
    fillColor(0);
}

void OctoWindow::setBgColor(uint32_t color) {
    myBgColor = color;
}

uint32_t OctoWindow::getBgColor()
{
    return myBgColor;
}

void OctoWindow::fillBgColor(){
    //Serial.printf("--fillBgColor 0x%X\n",myBgColor);
    fillColor(myBgColor);
}

////////////////////////////////
// Effects here
void OctoWindow::setNoEfx()
{
    efxDone = false;
    effectDelay = 0;
    curUpdateFunc = NULL;
    effectCount = 0;
    effectMaxCount=0;
    lastTime = 0;
}

void OctoWindow::setHoldEfx(int delayTime)
{
    printId();
    Serial.print("   set to use hold effect, delayMs ");Serial.println(delayTime);
    Serial.print("   currTime:");Serial.println(OctoWindow::currTime);
    setNoEfx();
    effectDelay = delayTime;
    curUpdateFunc = &OctoWindow::holdUpdateEfx;
}

void OctoWindow::setSolidColorEfx(uint32_t color, int holdTime)
{
//    Serial.println("setSolidColorEfx - fill");
    fillColor(color);
//    Serial.println("setSolidColorEfx - setHoldEfx");
    setHoldEfx(holdTime);
}

void OctoWindow::holdUpdateEfx(void)
{
    Serial.print("hold Update - we done  currMillis:");Serial.println(OctoWindow::currTime);
    // once we are called the hold time has passed so mark us as done
//    if (lastTime != 0)
//        efxDone = true;
//    else
//    {
//        Serial.println("holdEfx : lastTime is 0, need to run at least once");
        lastTime = OctoWindow::currTime;
//    }
    effectCount++;
    efxDone = true;
    Serial.printf("  lastTime: %ld efxCount %d done:%d=%d\n", lastTime, effectCount, efxDone, effectDone());
}

///////////////
void OctoWindow::setCircleEfx(uint32_t color, uint32_t delayTime, int count, int direction)
{
//    printId(); Serial.println("    set to use circle effect");
//    Serial.print("     color: "); Serial.print(color);
//    Serial.print(" delayTime: "); Serial.print(delayTime);
//    Serial.print(" count: "); Serial.print(count);
//    Serial.print(" direction: "); Serial.print(direction);
//    Serial.println();
    setNoEfx(); // reset values
    effectDelay = delayTime;
    curUpdateFunc = &OctoWindow::circleUpdateEfx;
    
    //fillBlack();
    fillBgColor();
    
    // starting a Circle Effect using color and time
    circle_color = color;
    circle_direction = direction;
    effectMaxCount = count; // default is once = 1
    
    // setup the initial frame
    if (circle_direction == 0) {
        circle_cursor = myStartPixel;
        myStrip->setPixelColor(circle_cursor, circle_color); //set rest of window to black
       for (int i=circle_cursor+1; i< myEndPixel; i++)
            myStrip->setPixelColor(i, myBgColor); //set rest of window to black
    } else {
        circle_cursor = myEndPixel;
        myStrip->setPixelColor(circle_cursor, circle_color); //set rest of window to black
        for (int i=circle_cursor-1; i >= myStartPixel; i--)
            myStrip->setPixelColor(i, myBgColor); //set rest of window to black
    }
    myStrip->setPixelColor(circle_cursor, circle_color);
    
    myStrip->setStripChanged(); // mark the strip changed
//    Serial.println("setCircleEfx end");
}

void OctoWindow::circleUpdateEfx(void)
{
    // we assume the update function has determined if it is time to call me
    
//      Serial.println("Updating Circle Effect");
//      Serial.print("   circle_cursor: ");Serial.println(circle_cursor);
      //printData();

    // circle moves a single pixel of circle_color around the virtual circle of the window
    // clear the currentPixel
    myStrip->setPixelColor(circle_cursor, myBgColor);
//fillBlack();
    
    if (circle_direction == 0) {
        circle_cursor++;
        if (circle_cursor > myEndPixel) {
            circle_cursor = myStartPixel;
            effectCount++;
//            Serial.println(" wrapped circle fwd");
        }
    } else {// direction ==1
        circle_cursor--;
        if (circle_cursor < myStartPixel) {
            circle_cursor = myEndPixel;
            effectCount++;
//            Serial.println(" wrapped circle rev");
        }
    }
    if (effectCount >= effectMaxCount) {
        efxDone = true;
    } else
        myStrip->setPixelColor(circle_cursor, circle_color);
}

void OctoWindow::setWipeEfx(uint32_t color, uint32_t delayTime, int count) // Wipe color once around window
{
    //  printId(); Serial.println("    set to use wipe effect");
    setNoEfx(); // reset values
    wipeDoneOne = false;

    effectDelay = delayTime;
    effectMaxCount = count;
    curUpdateFunc = &OctoWindow::wipeUpdateEfx;
    
    // starting a Circle Effect using color and time
    wipe_color = color;
    wipe_cursor = myStartPixel;
    wipe_direction = 0;// fwd
    
    fillBgColor();
    myStrip->setPixelColor(wipe_cursor, wipe_color);
    // dont change rest of colors
    //  for (int i=circle_cursor+1; i< myEndPixel; i++)
    //    myStrip->setPixelColor(i, 0); //set rest of window to black
    
    myStrip->setStripChanged(); // mark the strip changed
}

void OctoWindow::setReverseWipeEfx(uint32_t color, uint32_t delayTime, int count) // Wipe color once around window
{
    //  printId(); Serial.println("    set to use wipe effect");
    setNoEfx(); // reset values
    wipeDoneOne = false;

    effectDelay = delayTime;
    curUpdateFunc = &OctoWindow::wipeUpdateEfx;
    effectMaxCount = count;
    
    // starting a Circle Effect using color and time
    wipe_color = color;
    wipe_cursor = myEndPixel;
    wipe_direction = 1;// reverse
    fillBgColor();

    myStrip->setPixelColor(wipe_cursor, wipe_color);
    // dont change rest of colors
    //  for (int i=circle_cursor+1; i< myEndPixel; i++)
    //    myStrip->setPixelColor(i, 0); //set rest of window to black
    
    myStrip->setStripChanged(); // mark the strip changed
}


void OctoWindow::wipeUpdateEfx(void)
{
    bool wipeComplete = false;
    // wipe fills the window one pixel each update, then sets Done
    if (wipe_direction == 0)
    {
        wipe_cursor++;
        if (wipe_cursor > myEndPixel)
        {
            wipeComplete = true;
            wipe_cursor = myStartPixel;
            fillBgColor();
        }
    } else
    {
        wipe_cursor--;
        if (wipe_cursor < myStartPixel) {
            wipeComplete = true;
            wipe_cursor = myEndPixel;
            fillBgColor();
        }
    }
    myStrip->setPixelColor(wipe_cursor, wipe_color);

    if (wipeComplete) {
        effectCount++;
        if (effectCount > effectMaxCount)
        {
            efxDone = true;
            effectCount = 0;
        }
    }
}

//static uint32_t randomColor(uint32_t fromColor, uint32_t toColor)
//{
//  uint8_t fromR, toR;
//  uint8_t fromG, toG;
//  uint8_t fromB, toB;
//  fromR = OctoStrip::getRed(fromColor);
//  fromG = OctoStrip::getGreen(fromColor);
//  fromB = OctoStrip::getBlue(fromColor);
//
//  toR = OctoStrip::getRed(toColor);
//  toG = OctoStrip::getGreen(toColor);
//  toB = OctoStrip::getBlue(toColor);
//
//  return OctoStrip::color(random(fromR,toR), random(fromG,toG),random(fromB,toB));
//}

void OctoWindow::setRandomWipeEfx(uint32_t color1, uint32_t color2, uint32_t delayTime, int count) // Wipe color once around window
{
    //  printId(); Serial.println("    set to use random wipe effect");
    setNoEfx(); // reset values
    effectMaxCount = count;
    wipeDoneOne = false;

    effectDelay = delayTime;
    curUpdateFunc = &OctoWindow::randomWipeUpdateEfx;
    
    // starting a RandomWipe Effect using color and time
    randomWipeColor1 = color1;
    randomWipeColor2 = color2;
    wipe_color = OctoStrip::randomColor(color1, color2);
    wipe_cursor = myStartPixel;
    
    myStrip->setPixelColor(wipe_cursor, wipe_color);
    // dont change rest of colors
    //  for (int i=circle_cursor+1; i< myEndPixel; i++)
    //    myStrip->setPixelColor(i, 0); //set rest of window to black
    
    myStrip->setStripChanged(); // mark the strip changed
}


void OctoWindow::randomWipeUpdateEfx(void)
{
    // wipe fills the window one pixel each update, then sets Done
    
    myStrip->setPixelColor(wipe_cursor, wipe_color);
    wipe_cursor++;
    
    if (wipe_cursor > myEndPixel) {
        wipe_cursor = myStartPixel;
        wipe_color = OctoStrip::randomColor(randomWipeColor1, randomWipeColor2);
        wipeDoneOne = true;
    }
    if (wipeDoneOne) {
        effectCount++;
        fillBgColor();
        if (effectCount > effectMaxCount) {
            efxDone = true;
            effectCount = 0;
        }
    }

}

void OctoWindow::setBlinkEfx(uint32_t color, uint32_t delayTime, int count)
{
    // set the instance stuff
    setNoEfx(); // reset values
    
    effectDelay = delayTime;
    curUpdateFunc = &OctoWindow::blinkUpdateEfx;
    
    // effect specific stuff
    blink_color = color;
    blink_state = false;
    effectMaxCount = count;
    effectCount++;
    
    blinkUpdateEfx();
    
}

void OctoWindow::blinkUpdateEfx()
{
    if (blink_state)
    {
        fillColor(blink_color);
        blink_state = false;
    } else {
        //fillBlack();
        fillBgColor();
        blink_state = true;
        effectCount++;
    }
    
    if (effectMaxCount > 0 && effectCount > effectMaxCount)
        efxDone = true;
}

// couple flags for the state
static const int sparkleFLASH = 1;
static const int sparkleTWEEN = 0;

void OctoWindow::setSparkleEfx(uint32_t color, int flashTime, int tweenTime, int count)
{
    setNoEfx(); // reset values
    
    effectDelay = flashTime;
    curUpdateFunc = &OctoWindow::sparkleEfxUpdate;
    
    sparkleColor = color;
    sparkleFlashTime = flashTime;
    sparkleTweenTime = tweenTime;
    effectMaxCount = count;
    sparkleState = sparkleFLASH;
    sparkleCurPixel= random(myStartPixel, myEndPixel);
    
    //fillBlack();
    fillBgColor();
    
    // now turn on just that pixel
    myStrip->setPixelColor(sparkleCurPixel,sparkleColor);
    myStrip->setStripChanged(); // mark the strip changed
}

void OctoWindow::sparkleEfxUpdate(void)
{
    if (sparkleState == sparkleFLASH) {
        // it is on, turn off and set to sparkleTWEEN
        myStrip->setPixelColor(sparkleCurPixel, myBgColor);
        sparkleState = sparkleTWEEN;
        effectDelay = sparkleTweenTime;
    } else {
        // it is in TWEEN, so turn to FLASH: select new pixel and turn it on
        sparkleCurPixel= random(myStartPixel, myEndPixel);
        myStrip->setPixelColor(sparkleCurPixel, sparkleColor);
        sparkleState = sparkleFLASH;
        effectDelay = sparkleFlashTime;
    }
    
    effectCount++;
    if (effectMaxCount > 0 && effectCount > effectMaxCount)
    {
        efxDone = true;
    }
}
////////////////////////////////
void OctoWindow::clearActive(void)
{
    for (int i= myStartPixel; i <= myEndPixel; i++)
        myStrip->setPixelInactive(i);
}

/*! \brief setMultiSparkEfx: N randomly selected pixels in window flash on/off in selected color
 *  flashTime is time pixels are ON; Tween Time is time the are off
 *  when tweenTime is up, new pixels are chosen
 *  when there have been count flashes, the effect is set to Done.
 *  if color == 0, randomly select rainbow of colors
 */
void OctoWindow::setMultiSparkleEfx(uint32_t color, int flashTime, int tweenTime, int numActive, int count)
{
    setNoEfx(); // reset values
    
    effectDelay = flashTime;
    curUpdateFunc = &OctoWindow::multiSparkleEfxUpdate;
    
    multiSparkleColor = color;
    multiSparkleFlashTime = flashTime;
    multiSparkleTweenTime = tweenTime;
    effectMaxCount = count;
    multiSparkleNumActive = numActive;
    multiSparkleState = sparkleFLASH;
    
    if (numActive <= 0 || numActive > myPixelCount)
        numActive = myPixelCount;
    multiSparkleNumActive = numActive;
    
    clearActive();
    //fillBlack();
    fillBgColor();
    
    // Select N pixels, make them Active and set them to the color
    multiSparkleEfxSelectPixels();
    
    myStrip->setStripChanged(); // mark the strip changed
}

void OctoWindow::multiSparkleEfxSelectPixels()
{
    //  Serial.println("multiSparkleEfxSelectPixels");
    
    if (multiSparkleNumActive == 0 || multiSparkleNumActive == myPixelCount)
    {
        // do all pixels in window; skip random choice so it runs fast
        for (int idx=myStartPixel; idx<= myEndPixel; idx++) {
            myStrip->setPixelActive(idx);
            uint32_t color =multiSparkleColor;
            if (color == 0)
                color = OctoStrip::randomWheelColor();
            myStrip->setPixelColor(idx,color);
        }
    } else {
        for (int i = 0; i < multiSparkleNumActive; i++){
            int idx = random(myStartPixel,myEndPixel);
            while (myStrip->isPixelActive(idx))
                // if it is already active, pick another; note this could take a while to fill large number
                idx = random(myStartPixel,myEndPixel);
            //      Serial.print("Set Pixel Active: "); Serial.print(idx);Serial.println();
            myStrip->setPixelActive(idx);
            uint32_t color =multiSparkleColor;
            if (color == 0)
                color = OctoStrip::randomWheelColor();
            myStrip->setPixelColor(idx,color);
        }
    }
}

void OctoWindow::multiSparkleEfxUpdate(void)
{
    //  printId(); Serial.print("multiSparkle Update state:"); Serial.print(multiSparkleState); Serial.println();
    
    if (multiSparkleState == sparkleFLASH) {
        //    Serial.println("  clear and set to TWEEN");
        // it is on, turn off and set to multiSparkleTWEEN
        for (int idx = myStartPixel; idx <= myEndPixel;idx++) {
            if (myStrip->isPixelActive(idx))
                myStrip->setPixelColor(idx, myBgColor);
        }
        clearActive();
        multiSparkleState = sparkleTWEEN;
        effectDelay = multiSparkleTweenTime;
    } else {
        // it is in TWEEN, so turn to FLASH: select new pixels
        multiSparkleEfxSelectPixels();
        
        multiSparkleState = sparkleFLASH;
        effectDelay = multiSparkleFlashTime;
    }
    
    effectCount++;
    if (effectMaxCount > 0 && effectCount > effectMaxCount)
    {
        efxDone = true;
    }
    
    //  Serial.println("Bottom sparkle update");
    //  printData();
    //  delay(2000);
    
}

////////////////////
// Fade = linear fade between two colors, cycle makes if fade back
// linear fade in rgb space looks odd if not to black
//  better to fade in hsv or hcl
//  http://www.alanzucconi.com/2016/01/06/colour-interpolation/
//  but compute is expensive. might be better to precompute
//  might be better to have a WheelFade,
static const int fadeFadeIn = 0;
static const int fadeFadeOut = 1;

void OctoWindow::setFadeEfx(uint32_t fromColor, uint32_t toColor, int fadeTime, int type, int count)
{
    setNoEfx(); // reset values
    Serial.print("setFadeEfx from ");Serial.print(fromColor,HEX);
    Serial.print(" to ");Serial.print(toColor,HEX);
    Serial.print(" fadeTime ");Serial.print(fadeTime);
    Serial.print(" type ");Serial.print(type);
    Serial.print(" count ");Serial.print(count);
    Serial.println();
    
    effectDelay = fadeTime;
    curUpdateFunc = &OctoWindow::fadeEfxUpdate;
    
    fadeFromColor = fromColor;
    fadeToColor = toColor;
    fadeType = type;
    effectMaxCount = count;
    
    // internally we use seperate RGB values
    fadeFromR = myStrip->getRed(fromColor);
    fadeFromG = myStrip->getGreen(fromColor);
    fadeFromB = myStrip->getBlue(fromColor);
    
    fadeToR = myStrip->getRed(toColor);
    fadeToG = myStrip->getGreen(toColor);
    fadeToB = myStrip->getBlue(toColor);
    
    fadeCurR = fadeFromR;
    fadeCurG = fadeFromG;
    fadeCurB = fadeFromB;
    
    fadePhase = fadeFadeIn;
    fillColor(fromColor);
    
    myStrip->setStripChanged(); // mark the strip changed
}


void OctoWindow::fadeEfxUpdate(void)
{
//    Serial.println("fadeEfxUpdate");
//    Serial.print("  fadePhase");Serial.print(fadePhase);

    
    // uses ternary operator to handle incr/decr direction
    if (fadePhase == fadeFadeIn){
        // fade in, fancy way to linearly ramp each of RGB, regardless of whether from>to or from<to
        fadeCurR = ((fadeToR > fadeCurR) ? (fadeCurR+1) : ((fadeToR != fadeCurR) ? fadeCurR-1 : fadeCurR));
        fadeCurG = ((fadeToG > fadeCurG) ? (fadeCurG+1) : ((fadeToG != fadeCurG) ? fadeCurG-1 : fadeCurG));
        fadeCurB = ((fadeToB > fadeCurB) ? (fadeCurB+1) : ((fadeToB != fadeCurB) ? fadeCurB-1 : fadeCurB));
        
        // faded all the way in? are we cycling?
        if (fadeCurR == fadeToR && fadeCurG == fadeToG && fadeCurB == fadeToB) {
            switch (fadeType) {
                case fadeTypeCycle:
                    //printId(); Serial.println("FadeEfx: cycled all the way in; fade out");
                    // faded all in, cycle out
                    fadePhase = fadeFadeOut;
                    break;
                case fadeTypeJumpBack:
                    //printId(); Serial.println("FadeEfx: cycled all the way in; not cycle, jump back");
                    fadeEfxEndCheck();
                    fillColor(fadeFromColor);
                    fadeCurR = fadeFromR;
                    fadeCurG = fadeFromG;
                    fadeCurB = fadeFromB;
                case fadeTypeOnce:
                default:
                    //thats all
                    fadeEfxEndCheck();
                    break;
            }
        }
    } else { // fade out
        // fade in, fancy way to linearly ramp each of RGB, regardless of whether from>to or from<to
        fadeCurR = ((fadeFromR > fadeCurR) ? (fadeCurR+1) : ((fadeFromR != fadeCurR) ? fadeCurR-1 : fadeCurR));
        fadeCurG = ((fadeFromG > fadeCurG) ? (fadeCurG+1) : ((fadeFromG != fadeCurG) ? fadeCurG-1 : fadeCurG));
        fadeCurB = ((fadeFromB > fadeCurB) ? (fadeCurB+1) : ((fadeFromB != fadeCurB) ? fadeCurB-1 : fadeCurB));
        
        // faded all the way out?
        if (fadeCurR == fadeFromR && fadeCurG == fadeFromG && fadeCurB == fadeFromB) {
            //printId(); Serial.println("FadeEfx: cycled all the way out; fade in");
            fadePhase = fadeFadeIn;
            fadeEfxEndCheck();
        }
    }
    fillColor(myStrip->color(fadeCurR, fadeCurG, fadeCurB));
    
}

void OctoWindow::fadeEfxEndCheck()
{
    effectCount++;
//    Serial.print(" fade Efx Count ");Serial.println(effectCount);
    if (effectCount > effectMaxCount)
    {
        efxDone = true;
//        Serial.print(" fade Efx Done ");
    }
}

void OctoWindow:: setRainbowEfx(uint16_t waitTime, int type, int count)
{
    // members common to all effects
    setNoEfx();// reset counters
    effectDelay = waitTime;
    // effect specfic members
    curRainbowColor = 0;
    rainbowType = type;
    effectMaxCount = count;
    
    // and set the update function - magic of function poiners
    curUpdateFunc = &OctoWindow::rainbowEfxUpdate;
}

void OctoWindow::rainbowEfxUpdate(void)
{
    // put a color in each pixel of window, depending on type
    for (int i=0; i < myPixelCount; i++)
    {
        // colorWheel has 255 colors, from Adafruit NeoPixel strandtest example
        if (rainbowType == 0) {
            myStrip->setPixelColor(i+myStartPixel, OctoStrip::colorWheel((i+curRainbowColor) & 255));
        } else {
            myStrip->setPixelColor(i+myStartPixel, OctoStrip::colorWheel(((i*256/myPixelCount)+curRainbowColor) & 255));
        }
    }
    
    curRainbowColor++;
    if (curRainbowColor > 255)
    {
        // reached end of colors
        effectCount++;
        // set color index back to start so we can keep cycling
        curRainbowColor = 0;
        if (effectCount > effectMaxCount)
        {
            efxDone = true;
        }
    }
}

void OctoWindow::setPixelColor(int pixIdx, int color)
{
    int idx = myStartPixel + pixIdx;
    myStrip->setPixelColor(idx, color);
    myStrip->setStripChanged();
}

