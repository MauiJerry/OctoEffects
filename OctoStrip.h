
// OctoStrip
/**
 * @file OctoStrip.h
 * @brief defines OctoStrip class on top of PJRC OctoWS8211
 *
 *   a class derived from OctoWS2811
 *   adds a simple boolean stripChanged
 * This allows us to set/clear/test when we run a OctoWindow Effect
 * and only run the show function if we changed the strip
 * note that this requires all writes to use setStripChanged(),
 * which may not be true, if the code directly accesses strip.setPixel()
 * 
 * OctoStrip only one strip, with 8 rows on specific pins
 * differs greatly from Adafruit_NeoPixel
 */

#ifndef _OCTOSTRIP_
#define _OCTOSTRIP_

#include <OctoWS2811.h>

class OctoStrip: public OctoWS2811 {
private:
    static OctoStrip *singleton;
    uint8_t brightness;
    boolean stripChanged; /*!< marked TRUE by any effect that changes a pixel */
    static int s_nIDGenerator; /*! class level counter for next instance id */
    int myId; /*!< id of this strip; autogenerated by constructor */
    boolean *pixelActive; /*!<  array of booleans to define if each pixel is in use by Effects */
    /* instead of boolean, someday change this to more memory efficient technique
        https://github.com/RobTillaart/Arduino/tree/master/libraries/BitArray
        https://github.com/Chris--A/BitBool
     */
public:
    static OctoStrip* getInstance() { return singleton;}
    OctoStrip(uint32_t numPerStrip, void *frameBuf, void *drawBuf, uint8_t config = WS2811_GRB|WS2811_800kHz);
    int getId() {return myId;} /*!< returns autogen'd id of this strip */
    void setStripChanged(); /*!< marks the strip as Changed */
    void clearStripChanged(); /*!< clears changed flag; strip shown only if changed */
    boolean getStripChanged(); /*!< access internal flag */
    
    // some simple functions to effect whole strip
    void clearStrip(); /*!< clears whole strip to black */
    void fillStrip(uint32_t c); /*!< fills whole strip with Color */
    
    void printId(); /*!< uses Serial.println to emit strip number */
    
    void show(void); /*!< if strip changed, invoke parent show, and clear stripChanged */
    
    uint8_t getMaxIntensityUsed(); /*!< returns maximum value across all rgb pixels */
    //////
    /**
     * Active Pixel booleans can be used by OctoWindows to determine if a particular pixel
     * is active or not at any time.  Useful for sparkles, random fades, etc.
     */
    void clearActive(); /*!< clears the active pixel array */
    void setPixelActive(int idx); /*!< sets active bit for given pixel index */
    void setPixelInactive(int idx); /*!< clears active bit for given pixel index */
    boolean isPixelActive(int idx); /*!< returns whether current pixel is active */
    
    //////
    // helper function to create random Colors (unit32_t of r, g,b)
    /**
     * helper functions dealing with OctoWS2811::Color (24 bit color)
     * note that OctoWS2811 does not provide class method, so we do
     */
    static int Color(uint8_t red, uint8_t green, uint8_t blue)
    {
      return (red << 16) | (green << 8) | blue;
    }
    
    static uint32_t randomColor(void) 
    {
        return Color(random(0,255),random(0,255),random(0,255));
    }
    
    static uint32_t randomWheelColor(void) {
        return colorWheel(random(0,255));
    }
    static uint32_t randomColor(uint32_t c1, uint32_t c2);
    static uint8_t getRed(uint32_t c) { return (uint8_t)(c >> 16);}
    static uint8_t getGreen(uint32_t c) { return (uint8_t)(c >>  8);}
    static uint8_t getBlue(uint32_t c) { return (uint8_t)c;}
    
    static const uint32_t White; /*!< quick common reference for full white color */
    static const uint32_t Black; /*!< quick common reference for full black color */
    
    static uint32_t colorWheel(byte WheelPos);  /*!< colorWheel defines 255 colors of full intensity */
    
    // HueSaturationLightness to RGB
    static uint32_t hsl(uint16_t ih, uint8_t is, uint8_t il); /*!< hsl creates a packed RGB value given Hue Saturation Lightness parameters */
private:
    static uint8_t hsl_convert(float c, float t1, float t2);

  // mirror of drawbuffer pointer since Octo doesnt expose it
    void *drawBuffer;
    int bufferSize() { return numPixels()*6;}
    
public :
  // functions to cover for missing Adafruit_NeoPixel functions from OctoWS2811
  void setBrightness(uint8_t b ) ;
  uint8_t getBrightness(void);
    void setPixelColor(int pixel, int color);
  int getPixelColor(uint32_t pixel) { return getPixel(pixel);}

  static int getAbsolutePixel(int row, int col);

};
#endif // OCTOSTRIP