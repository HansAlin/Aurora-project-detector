#include "Arduino.h"
#include <Wire.h>


#ifndef CloudCover_h
#define CloudCover_h

class CloudCover {
  public:
    CloudCover();
    void begin();
    float get_cloud_value(float temp_adjust, float w_1);

  private:
      


};
#endif