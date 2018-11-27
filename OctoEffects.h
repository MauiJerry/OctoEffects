// OctoEffects library header... include the important components and dependencies
// revision of NeoEffects to switch from Adafruit to PJRC led library
// NeoEffects provides multiple simultaneous lighting pattern effects on a NeoPixel (rgb led strip using ws2812)
// NeoPixel examples generally use delay() which precludes simultaneous effects

#ifndef OctoEFFECTS_LIBRARY
#define OctoEFFECTS_LIBRARY

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <pins_arduino.h>
#endif

// We were based on the Adafruit library: https://github.com/adafruit/Adafruit_NeoPixel
// but repurposed to PJRC OctoWS2811 library base
//    https://www.pjrc.com/teensy/td_libs_OctoWS2811.html
// This has serious constraints on the effects as OctoWS2811 
//  only supports 8 strips on specific pins of a Teensy3.x
// That lets the library use DMA and other features a generic setup could not
// CAVEAT: i have not updated the comments for the underlying change
//
#include <OctoWS2811.h>

// we add two classes:
// OctoStrip provides an abstraction for a daisy chain of NeoPixels, 
//    allowing multiple effects to share memory buffer and write only once per loop(), if needed
#include "OctoStrip.h"

// OctoWindow defines a window (range of pixels) within a strip on which an effect will be produced
//    windows can be distinct or overlap
//    a base set of effect functions is included in the library
//    additional effects can be implemented via subclassing
#include "OctoWindow.h"

#endif
