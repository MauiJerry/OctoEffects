// DazzleEffect
#include "OctoWindow.h"

/*
 * void OctoWindow::setDazzleEfx( int flashTime, int probOff, int count)
 * @param flashTime time delay tween changes
 * @param probOff  probability pixel is changes state = 0-100
 * @param count number of times to sparkle before marking effect DONE
 */
void OctoWindow::setDazzleEfx( int flashTime, int probOff, int count)
{
  setNoEfx(); // reset values
    
  effectDelay = flashTime;
  curUpdateFunc = (OctoWindowUpdateFunc) &OctoWindow::dazzleEfxUpdate;

  dazzleProbChange = probOff;
  dazzleMaxCount = count;
  
  clearActive();
  fillBlack(); // clear it
  //fillBgColor();

  myStrip->setStripChanged(); // mark the strip changed

}

void OctoWindow::dazzleEfxUpdate(void)
{
  // if pixel is ON, give a random P to turn it off
  // if pixel is OFF, give random P to turn it on
  for (int i=myStartPixel; i<= myEndPixel; i++) 
  {
    bool changeState = (random(0,100) < dazzleProbChange);
    if (!changeState)
      continue;
    if (myStrip->isPixelActive(i))
    {
      myStrip->setPixelInactive(i);
        myStrip->setPixelColor(i,OctoStrip::Black);
    } else 
    {
      myStrip->setPixelActive(i);
      myStrip->setPixelColor(i,OctoStrip::randomWheelColor());    
    }
  }

  effectCount++;
  if (dazzleMaxCount > 0 && effectCount > dazzleMaxCount)
  {
    efxDone = true;
  }

}
