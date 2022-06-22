#include "Arduino.h"
#include "CloudCover.h"

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

CloudCover::CloudCover(int DHTPIN): dht(DHTPIN, DHT11) {
  
  Adafruit_MLX90614 mlx = Adafruit_MLX90614();
  float measured_sky_temp = 0;
  float calculated_sky_temp = 0;
  float cloud_value = 0;
  float ambientTemp = 0;
  float objectTemp = 0;
  
}

void CloudCover::begin() {
  dht.begin();
  
  // init sensor
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  }
  
}

float CloudCover::clear_sky_temp() {
  /**
   * @brief Calculates the clear sky temperature
   * @arg no arguments
   * @return return the calculated value of the clear sky temperature
   * based on temperature and humidity
   * 
   */

  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();
  float sky_temp;
  float vapor_pressure;

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return calculated_sky_temp;  // Last measured temperatur
  }
  else {  
    vapor_pressure = humidity/100*6.11*pow(10,((7.5*temperature)/(273.3+temperature)));
    calculated_sky_temp = -31.1 + 0.417*temperature*sqrt(vapor_pressure);  
    Serial.println("Vapor pressure: " + String(vapor_pressure) + "mb");
    Serial.println("Clear sky temperature: " + String(sky_temp) + "C ");

  }

  return calculated_sky_temp;
}


float CloudCover::get_cloud_value(float cloud_value_scale=30) {
  /**
   * @brief This function calculate the cloude value
   * @return:
   *  cloud_value: float between 0 - 1.0
   * @argument:
   *  cloud_value_scale: the largest possible difference between measured 
   *  sky temperature and calculated sky temperature
   * 
   */
  float x = mlx.readAmbientTempC();
  if (!isnan(x)) { // On success, read() will return 1, on fail 0.
    Serial.println("Ambient temperature: " + String(x));
    ambientTemp = x;  // Not used
  }
  x= mlx.readObjectTempC();
  if ( !isnan(x)) {
    
    objectTemp = x;
    Serial.println("Sky temperature: " + String(objectTemp));
  }
  calculated_sky_temp = clear_sky_temp();

  float temp_diff = objectTemp - calculated_sky_temp;
  float cloud_factor = temp_diff/cloud_value_scale;
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