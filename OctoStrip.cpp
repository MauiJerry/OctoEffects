// OctoStrip.cpp
// a wrapper/dervied class of Adafruit_NeoPixel to add some functionality
// most everything passes on to the parent class

#include <OctoWS2811.h>
#include "OctoStrip.h"

OctoStrip* OctoStrip::singleton = 0;

int OctoStrip::pixPerRow;
int OctoStrip::s_nIDGenerator = 1;
const uint32_t OctoStrip::White = OctoStrip::Color(255,255,255);
const uint32_t OctoStrip::Black = OctoStrip::Color(0,0,0);

// wrapper on OctoWS2811 constructor
OctoStrip::OctoStrip(uint32_t numPerStrip, void *frameBuf, void *drawBuf, uint8_t config) 
 : OctoWS2811(numPerStrip, frameBuf, drawBuf, config)
{
  int countPixels = numPerStrip*8;
  pixPerRow = numPerStrip;
    
  if (singleton != NULL)
    {
        Serial.println("WARNING - multiple OctoStrip declared");
    }
  stripChanged = false;
  myId = s_nIDGenerator++;
  // allocate and initialize array of active flags
  // have this at Strip level vs Window to reduce #dynamic memory alloc
  pixelActive = new boolean[countPixels];
  for (int i=0; i<countPixels; i++)
      pixelActive[i]=false; // memset would be faster

  // keep a reference to drawbuffer used by OctoWS2811
  if (drawBuf)
    drawBuffer = drawBuf;
  else
    drawBuffer = frameBuf;
    
  singleton = this;
}

void OctoStrip::printId(void)
{
  Serial.print("Strip  "); 
  Serial.print(myId); Serial.print(" ");
}


// overload the base class show to check if stripChanged
void OctoStrip::show(void)
{
    if (!stripChanged) {
//      printId(); Serial.println(" Strip did not Changed");
      return;
    }
//    printId(); Serial.println("Strip Changed, show it");
    
    OctoWS2811::show();
    
    stripChanged = false;
}

 void OctoStrip::setStripChanged()
 {
//  printId();Serial.println("  Setting changed ");
    stripChanged = true;
//  Serial.println("  StripChanged ");
}
 
 void OctoStrip::clearStripChanged()
 {
//  printId();Serial.println("  Setting cleared ");
    stripChanged = false;
 }
 
 boolean OctoStrip::getStripChanged()
 {
    return stripChanged;
 }

// clear to background?
 void OctoStrip::clearStrip()
 {
    // very fast clear to zero but we need to know the strip memory and size
    // which we recorded when created = numPixels/strip*8strips * 6bytes/pixel
   //memset(drawBuffer, 0, bufferSize());

     fillStrip(0);
   //setStripChanged();
 }
 
void OctoStrip::fillStrip(uint32_t c)
{
  for (int i=0;i < numPixels();i++)
    setPixelColor(i, c);
  setStripChanged();
}

void OctoStrip::clearActive()
{
    for (int i=0;i < numPixels();i++)
       setPixelInactive(i);
}

void OctoStrip::setPixelActive(int idx)
{
  pixelActive[idx] = true;
}

void OctoStrip::setPixelInactive(int idx)
{
  pixelActive[idx] = false;
}

boolean OctoStrip::isPixelActive(int idx)
{
  return pixelActive[idx];
}
  
uint32_t OctoStrip::randomColor(uint32_t fromColor, uint32_t toColor)
{
  uint8_t fromR, toR;
  uint8_t fromG, toG;
  uint8_t fromB, toB;
  fromR = OctoStrip::getRed(fromColor);
  fromG = OctoStrip::getGreen(fromColor);
  fromB = OctoStrip::getBlue(fromColor);
  
  toR = OctoStrip::getRed(toColor);
  toG = OctoStrip::getGreen(toColor);
  toB = OctoStrip::getBlue(toColor);
  
  return OctoStrip::Color(random(fromR,toR), random(fromG,toG),random(fromB,toB));
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t OctoStrip::colorWheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return OctoStrip::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return OctoStrip::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return OctoStrip::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

//
uint8_t OctoStrip::getMaxIntensityUsed()
{
    uint8_t max = 0;
    // uses getPixelColor() which is slow but deals with RGBW, and brightness
    uint16_t pixelCount = numPixels();
    for (uint16_t i = 0; i < pixelCount;i++)
    {
        uint32_t color = getPixelColor(i);
        // unpack and check vs max
        uint8_t c = (color >> 16) & 0xFF;
        if (c > max) max = c;
        c = (color >> 8) & 0xFF;
        if (c > max) max = c;
        c =  color & 0xFF;
        if (c > max) max = c;
    }
    return max;
}


// Adjust output brightness; 0=darkest (off), 255=brightest.  This does
// NOT immediately affect what's currently displayed on the LEDs.  The
// next call to show() will refresh the LEDs at this level.  However,
// this process is potentially "lossy," especially when increasing
// brightness.  The tight timing in the WS2811/WS2812 code means there
// aren't enough free cycles to perform this scaling on the fly as data
// is issued.  So we make a pass through the existing color data in RAM
// and scale it (subsequent graphics commands also work at this
// brightness level).  If there's a significant step up in brightness,
// the limited number of steps (quantization) in the old data will be
// quite visible in the re-scaled version.  For a non-destructive
// change, you'll need to re-render the full strip data.  C'est la vie.
//
// ripped off Adafruit_NeoPixel and enforced only in this class
// does NOT rescale
void OctoStrip::setBrightness(uint8_t b) {
  // Stored brightness value is different than what's passed.
  // This simplifies the actual scaling math later, allowing a fast
  // 8x8-bit multiply and taking the MSB.  'brightness' is a uint8_t,
  // adding 1 here may (intentionally) roll over...so 0 = max brightness
  // (color values are interpreted literally; no scaling), 1 = min
  // brightness (off), 255 = just below max brightness.
  uint8_t newBrightness = b + 1;
  if(newBrightness != brightness) { // Compare against prior value
    // Brightness has changed -- re-scale existing data in RAM
    // OctoStrip - scaling not implemented for existing, redraw after setBrightness
//    uint8_t  c,
//            *ptr           = pixels,
//             oldBrightness = brightness - 1; // De-wrap old brightness value
//    uint16_t scale;
//    if(oldBrightness == 0) scale = 0; // Avoid /0
//    else if(b == 255) scale = 65535 / oldBrightness;
//    else scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;
//    for(uint16_t i=0; i<numBytes; i++) {
//      c      = *ptr;
//      *ptr++ = (c * scale) >> 8;
//    }
    brightness = newBrightness;
  }
}

void OctoStrip::setPixelColor(int pixel, int c)
{
    int c2 = c;
    if(brightness)
    { // See notes in setBrightness()
        uint8_t r = (uint8_t)(c >> 16);
        uint8_t g = (uint8_t)(c >>  8);
        uint8_t b = (uint8_t)c;
        r = (r * brightness) >> 8;
        g = (g * brightness) >> 8;
        b = (b * brightness) >> 8;
        c2 = color(r,g,b);
    }
    // now call
//    Serial.printf("setPixelColor %d 0x%x\n", pixel, c2);
   setPixel(pixel,c2);
    stripChanged = true;

}

int OctoStrip::getAbsolutePixel(int row, int col)
{
    return getPixelAtRowCol(row, col);
}
int OctoStrip::getPixelAtRowCol(int row, int col)
{
    // should check if this is col is next row? nah no safety
    int absPixNum = (getPixPerRow() * row) + col;
    Serial.printf(" row %d col %d => %d\n", row, col,absPixNum);
    return absPixNum;
}
