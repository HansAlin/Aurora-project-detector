/* 
 *  OPT3001.h - library creates instances of OPT3001 sensors and 
 *  takes care of the interaction between microcontroler and sensor
 *  Created by Hans Alin, Mora, 8 February 2022
 */

#include "Arduino.h"
#ifndef OPT3001_h
#define OPT3001_h

 
class OPT3001 {
  public:
    OPT3001(int SDA, int SCL, byte address);
    float recive_data();

  private:
    float converter(uint16_t data);
    uint16_t  _exponent;
    uint16_t  _decimal;
    float _flux;
    uint16_t  _iData;
    uint8_t   _iBuff[2];
    int _SDA;
    int _SCL;
    byte _address;
    
};
#endif