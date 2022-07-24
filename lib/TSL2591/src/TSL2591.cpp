/*
   @file     Adafruit_TSL2591.cpp
    @author   KT0WN (adafruit.com)
    This is a library for the Adafruit TSL2591 breakout board
    This library works with the Adafruit TSL2591 breakout
    ----> https://www.adafruit.com/products/1980
    Check out the links above for our tutorials and wiring diagrams
    These chips use I2C to communicate
    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!
    @section LICENSE
    Software License Agreement (BSD License)
    Copyright (c) 2014 Adafruit Industries
    All rights reserved.
*/

/* TSL2591 Digital Light Sensor */
/* Dynamic Range: 600M:1 */
/* Maximum Lux: 88K */
#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "SPI.h"
#include "TSL2591.h"


// Example for demonstrating the TSL2591 library - public domain!

// connect SCL to I2C Clock
// connect SDA to I2C Data
// connect Vin to 3.3-5V DC
// connect GROUND to common ground

TSL2591::TSL2591() {
  Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // 2591 pass in a number for the sensor identifier (for your use later)
} 

void TSL2591::begin(TwoWire &theWire, uint8_t addr) {
  I2C_wire = &theWire;
  Serial.println(F("Starting Adafruit TSL2591 Test!"));
  
  if (tsl.begin(I2C_wire, addr)) 
  {
    Serial.println(F("Found a TSL2591 sensor"));
  } 
  else 
  {
    Serial.println(F("No sensor found ... check your wiring?"));
    while (1);
  }
}

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void TSL2591::displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" lux"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" lux"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution, 4); Serial.println(F(" lux"));  
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(500);
}

/**************************************************************************/
/*!
    @brief Configures the gain and integration time for the TSL2591
    @param gain, possible values: 1, 25, 428, 9876, dimmer light higer value
    @param intTime, possible values: 100, 200, 300, 400, 500, 600, // Changing 
    the integration time gives you a longer time over which to sense light
    longer timelines are slower, but are good in very low light situtations!
*/
/**************************************************************************/
void TSL2591::configureSensor(int gain, int intTime)
{ 
  
  
  switch (gain)
  {
  case 1:
    tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
    break;
  case 25:
    tsl.setGain(TSL2591_GAIN_MED);    // 25x gain (bright light)
    break;
  case 428:
    tsl.setGain(TSL2591_GAIN_HIGH);    // 428x gain (bright light)
    break;
  case 9876:
    tsl.setGain(TSL2591_GAIN_MAX);      // 9876x gain
    break;
  default:
    tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
    break;  
  }

  switch (intTime)
  {
  case 100:
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
    break;
  case 200:
    tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
    break;
  case 300:
    tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
    break;
  case 400:
    tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
    break;
  case 500:
    tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);  
    break;
  case 600:
    tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)  
    break;
  default:
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
    break;
  }

  /* Display the gain and integration time for reference sake */  
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Gain:         "));
  tsl2591Gain_t _gain = tsl.getGain();
  switch(_gain)
  {
    case TSL2591_GAIN_LOW:
      Serial.println(F("1x (Low)"));
      break;
    case TSL2591_GAIN_MED:
      Serial.println(F("25x (Medium)"));
      break;
    case TSL2591_GAIN_HIGH:
      Serial.println(F("428x (High)"));
      break;
    case TSL2591_GAIN_MAX:
      Serial.println(F("9876x (Max)"));
      break;
  }
  Serial.print  (F("Timing:       "));
  Serial.print((tsl.getTiming() + 1) * 100, DEC); 
  Serial.println(F(" ms"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
}

/**************************************************************************/
/*
    Shows how to perform a basic read on visible, full spectrum or
    infrared light (returns raw 16-bit ADC values)
// TODO Test if simmple read works different than advanced read
/**************************************************************************/
float TSL2591::simpleRead()
{
  // Simple data read example. Just read the infrared, fullspecrtrum diode 
  // or 'visible' (difference between the two) channels.
  // This can take 100-600 milliseconds! Uncomment whichever of the following you want to read
  uint16_t x = tsl.getLuminosity(TSL2591_VISIBLE);
  //uint16_t x = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2591_INFRARED);

  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("Luminosity: "));
  Serial.println(x, DEC);
  float lux = (float) x;
  return lux;
}


/**************************************************************************/
/*
    Show how to read IR and Full Spectrum at once and convert to lux
*/
/**************************************************************************/
void TSL2591::advancedRead(float * pvalues)
{
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  float lux = tsl.calculateLux(full, ir);
  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("IR: ")); Serial.print(ir);  Serial.print(F("  "));
  Serial.print(F("Full: ")); Serial.print(full); Serial.print(F("  "));
  Serial.print(F("Visible: ")); Serial.print(full - ir); Serial.print(F("  "));
  Serial.print(F("Lux: ")); Serial.println(lux, 6);
  pvalues[0] = lux;
  pvalues[1] = (float) ir;
  pvalues[2] = (float) full;
  
}

void TSL2591::sleep() {
  tsl.disable();
}

void TSL2591::awake() {
  tsl.enable();
}

