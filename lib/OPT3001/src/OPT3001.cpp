#include "Arduino.h"
#include "OPT3001.h"
#include <Wire.h>


OPT3001::OPT3001(int SDA, int SCL, byte address) {
  _SDA = SDA;
  _SCL = SCL;
  _address = address;
  

  Wire.begin(SDA, SCL);           //  SDA 0, SCL 2
  Wire.beginTransmission(address);// 1000 100  the adress to one of the sensors
  Wire.write(0x01);               //  1 Master will send data to sensor    
  Wire.write(0xCE);               //  1100 1110 configurations on OPT3001
  Wire.write(0X10);               //  0001 1000     1100 1110 0001 0000
  Wire.endTransmission();
}

float OPT3001::converter( uint16_t data) {
  _decimal = data & 0x0FFF;
  _exponent = (data & 0xF000) >> 12;
  return _decimal * (0.01 * pow(2, _exponent));
}

float OPT3001::recive_data() {
  Wire.beginTransmission(_address);
  Wire.write(0x00);           // Master ecpect data from slave
  Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom(_address, _SCL);
  //uint16_t  iData;
  //uint8_t   iBuff[2];
  //float flux;

  while (Wire.available()) {
    Wire.readBytes(_iBuff, SCL);
    //Serial.println(_iBuff[0], BIN);
    //Serial.println(_iBuff[1], BIN);
    _iData = (_iBuff[0] << 8) | _iBuff[1];
    //Serial.print(iData, BIN);
    //Serial.print("\t\t");
    _flux = converter(_iData);
    }
  return _flux;
}