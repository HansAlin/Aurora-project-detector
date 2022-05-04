#include "Arduino.h"
#include <Wire.h>
#include "CloudCover.h"
#include "Adafruit_MLX90614.h"
#include <Adafruit_I2CDevice.h>
#include "SPI.h"


CloudCover::CloudCover() {
  
  
}

void CloudCover::begin() {
  
  _mlx = mlx;
  // init sensor
  if (!_mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };
  _mlx.writeEmissivity(0.8);
}

float CloudCover::get_cloud_value(float temp_adjust=202, float w_1=2) {
  float ambientTemp = _mlx.readAmbientTempC();
  float objectTemp = _mlx.readObjectTempC();
  float clear_sky_value;
  // To Kelvin
  ambientTemp = ambientTemp + 273;
  objectTemp = objectTemp + 273;

  float tempdiff = objectTemp - ambientTemp;
  if (tempdiff < 0) {
    clear_sky_value = 0;
  }
  else {
    clear_sky_value = tempdiff / (objectTemp - temp_adjust ) * w_1;
    if(clear_sky_value > 1) {
      clear_sky_value = 1;
    }

  }

  return clear_sky_value;

}