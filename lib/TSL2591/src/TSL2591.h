/* TSL2591 Digital Light Sensor */
/* Dynamic Range: 600M:1 */
/* Maximum Lux: 88K */
#include "Arduino.h"
//#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "SPI.h"

#ifndef TSL2591_h
#define TSL2591_1

// Example for demonstrating the TSL2591 library - public domain!

// connect SCL to I2C Clock
// connect SDA to I2C Data
// connect Vin to 3.3-5V DC
// connect GROUND to common ground

class TSL2591 {
  public:
    TSL2591();
    void begin();
    void displaySensorDetails(void);
    void configureSensor(int gain, int intTime);
    float simpleRead();
    void advancedRead(float * pvalues);
    void unifiedSensorAPIRead(void);
    void sleep();
    void awake();
    
  private:
    Adafruit_TSL2591 tsl;
    TwoWire *I2C_wire;
};
#endif