#include "Arduino.h"
#include <math.h>

#ifndef CloudCover_h
#define CloudCover_h

class CloudCover {

  public:
    
    CloudCover();
    float get_cloud_value(float cloud_value_scale, float _humidity, float _temperature, float object_temp, float ambient_temp);
    // TODO float object_temp_adjust,
    float get_sensor_temp();
    float get_humidty();
    void sleep();
    void wake();
    float get_object_temp();
    float get_ambient_temp();
    float get_calculated_sky_temp();

  private:
      float clear_sky_temp();
      float get_emissivity();
      float measured_sky_temp;
      float calculated_sky_temp;
      float cloud_value;
      float temperature;
      float humidity;
      float ambientTemp;
      float objectTemp;
      int SDA;
      int SCL;
      double new_emissivity;
      
      


};
#endif