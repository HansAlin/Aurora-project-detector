#include "Arduino.h"
#include "CloudCover.h"
#include <math.h>


/***********************************************************
 * This library takes care of calculating cloud values from the cloud
 * sensr (MLX90614 and DHT11)
 * The equation for transform the humidty value to vapor pressure comes from:
 * https://www.weather.gov/epz/wxcalc_vaporpressure
 * The equation that calculates the clear sky temperature is taken from:
 * "Design and development of a simple infrared monitor for cloud detection", by
 * Maghrabi, A. and Clay, R. and Wild, N. and Dawson, B.
 ***********************************************************/

 //TODO no default constructor exists for class "DHT"!!

CloudCover::CloudCover() {
  
  
  float measured_sky_temp = 0;
  float calculated_sky_temp = 0;
  float cloud_value = 0;
  float ambientTemp = 0;
  float objectTemp = 0;
  float humidity = 0;
  float temperature = 0;
  double new_emissivity = 0.95;
  
}


float CloudCover::clear_sky_temp() {
  /**
   * @brief Calculates the clear sky temperature
   * @arg no arguments
   * @return return the calculated value of the clear sky temperature
   * based on temperature and humidity
   * 
   */

 
  float vapor_pressure;
 
  vapor_pressure = humidity/100*6.11*pow(10,((7.5*temperature)/(273.3+temperature)));
  calculated_sky_temp = -31.1 + 0.417*temperature*sqrt(vapor_pressure);  
  Serial.println("Vapor pressure: " + String(vapor_pressure) + "mb");
  Serial.println("Clear sky temperature: " + String(calculated_sky_temp) + "C ");
  
  return calculated_sky_temp;
}


float CloudCover::get_cloud_value(float cloud_value_scale=0.6, float object_temp_adjust=0.0, float _humidity=0.0, float _temperature=0.0, float object_temp=999.0, float ambient_temp=999.0 ) {
  /**
   * @brief This function calculate the cloude value
   * @return:
   *  cloud_value: float between 0 - 1.0
   * @argument:
   *  cloud_value_scale: the largest possible difference between measured 
   *  sky temperature and calculated sky temperature
   * 
   */
  humidity = _humidity;
  temperature = _temperature;
  objectTemp = object_temp;
  ambientTemp = ambient_temp;

  calculated_sky_temp = clear_sky_temp();

  float temp_diff = objectTemp - calculated_sky_temp;
  float cloud_factor = temp_diff/(15 + cloud_value_scale*25); // Scaling between 15 to 40 degrees 
  if (cloud_factor < 0) {
    Serial.println("Cloud value: " + String(1));
    return 1;
  }
  else if (cloud_factor > 1) {
    Serial.println("Cloud value: " + String(0));
    return 0;
  }
  else {
    Serial.println("Cloud value: " + String(1 - cloud_factor));
    return 1 - cloud_factor;
  }

}

float CloudCover::get_sensor_temp() {
  return temperature;
}

float CloudCover::get_humidty() {
  return humidity;
}

float CloudCover::get_object_temp() {
  return objectTemp;
}

float CloudCover::get_ambient_temp() {
  return ambientTemp;
}

float CloudCover::get_calculated_sky_temp() {
  return calculated_sky_temp;
}

// void CloudCover::sleep() {
//   Wire.beginTransmission(byte(0x5A));
//   Wire.write(byte(0x00));
//   Wire.write(byte(0xFF));
//   Wire.write(byte(0xE8));
//   Wire.endTransmission();

//   pinMode(SCL, OUTPUT);
// 	digitalWrite(SCL, LOW);
// 	pinMode(SDA, INPUT);
// }

// void CloudCover::wake() {
//   Wire.endTransmission(true);
//   pinMode(SCL, INPUT); // SCL high
// 	pinMode(SDA, OUTPUT);
// 	digitalWrite(SDA, LOW); // SDA low
// 	delay(50); // delay at least 33ms
// 	pinMode(SDA, INPUT); // SDA high
// 	delay(250);
// 	// PWM to SMBus mode:
// 	pinMode(SCL, OUTPUT);
// 	digitalWrite(SCL, LOW); // SCL low
// 	delay(10); // Delay at least 1.44ms
// 	pinMode(SCL, INPUT); // SCL high
//}