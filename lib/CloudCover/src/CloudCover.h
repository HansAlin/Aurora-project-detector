#include "Arduino.h"
//#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_MLX90614.h>
#include "SPI.h"
#include <Adafruit_Sensor.h>
#include "DHT.h"


#ifndef CloudCover_h
#define CloudCover_h


class CloudCover {
  DHT dht;
  public:
    
    CloudCover(int DHTPIN);
    void begin(int sda, int scl);
    float get_cloud_value(float cloud_value_scale);
    float get_sensor_temp();
    float get_humidty();
    void sleep();
    void wake();

  private:
      float clear_sky_temp();
      Adafruit_MLX90614 mlx;
      
      float measured_sky_temp;
      float calculated_sky_temp;
      float cloud_value;
      float temperature;
      float humidity;
      float ambientTemp;
      float objectTemp;
      int SDA;
      int SCL;
      
      


};
#endif