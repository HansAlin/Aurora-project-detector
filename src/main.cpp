#include <Arduino.h>
// ----------(c) Electronics-project-hub-------- //
//        Program for ESP8266 D1 MINI
//        
//#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "TSL2591.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "SPI.h"
#include <Adafruit_MLX90614.h>
#include <SparkFunMLX90614.h>
#include "DHT.h"

//#include "CloudCover.h"
#include "AuroraPoints.h"

//#include "Adafruit_GFX.h"
#include "TSpeak.h" 

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11


//------- WI-FI details ----------//
const char * ssid = "ASUS_50_2G"; //SSID here           AndroidAP             Xperia
const char * pass = "jugenjon"; // Passowrd here        l@ngdrag00          l@ngdrag00      
//--------------------------------// 

//----------- Channel 1       ----------------//
// Channel most for writing
unsigned long Channel_ID = 1636584; // Channel ID  collecting data
const char  * myWriteAPIKey = "LA81E3TBW06I54EY"; //Write API key
const char  * myReadAPIKey = "L6WSL9CU11VHP4NF";   //Read API key 
//-------------------------------------------//

//----------- Channel 2       ----------------//
// Channel for writing and reciving values from ThingSpeak
unsigned long Channel_ID_2 = 1669186; // Channel ID  collecting data
const char  * myWriteAPIKey_2 = "XQC5Y3Z9KFOCBM5R"; //Write API key
const char  * myReadAPIKey_2 = "LLKKI938S0S4M1X7";   //Read API key 
//-------------------------------------------//




const int dataPoints = 8;     // Number of datapoints to send to channel
const int dataPoints2 = 2;     // Number of datapoints to send to channel
float data[dataPoints];       // An array with all the datapoints
float data2[dataPoints2];

//int field_average_from_sensor = 1;
int read_field_value[] = {3,4,5,6,7,8};
const int read_data_length = 6;
float read_data[read_data_length];
int delayTime = 0;            // Initial delay time for microcontroler 
int sda = 4;                  // Pin on D1 MINI D2 on board ESP8266
int scl = 5;                  // Pin on D1 MINI D1 on board ESP8266
byte address = 0x29;          // I2c address to sensor
float sleeping;                   // If module going to sleep
int sleepmin = 1;            // TODO set to sleeps in 20 min
unsigned int raw;            // Reading value from A0 analog pin
float max_voltage = 4.1;      // Max voltage on battery
const int number_of_sensors = 3;
int night = 0;                    // Returning value from thingSpeak 0=day 1=
float temp_k;
float temp_b;
float _557_weight;
float fraction_weight;
float emissivity = 0.8;
float tempAC;
float tempOC;
float clear_sky_temp;


WiFiClient  client;

TSL2591 tsl2591[number_of_sensors] = {TSL2591(), TSL2591(), TSL2591()} ;

TSpeak thingSpeak;
TSpeak thingSpeak2;
DHT dht(DHTPIN, DHTTYPE);

Adafruit_MLX90614 mlx;
//IRTherm therm; // Create an IRTherm object to interact with throughout
AuroraPoints auror;


void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}

float clear_temp() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  float sky_temp;
  float vapor_pressure;
  // Check if any reads failed and exit early (to try again).

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    sky_temp = clear_sky_temp;
  }
  else {  
    // Taken from   https://www.weather.gov/epz/wxcalc_vaporpressure
    vapor_pressure = h/100*6.11*pow(10,((7.5*t)/(273.3+t)));
    // Taken from Detecting cloud with a simple infra red sensor
    sky_temp = -31.1 + 0.417*t*sqrt(vapor_pressure);  
    Serial.println("Vapor pressure: " + String(vapor_pressure) + "mb");
    Serial.println("Clear sky temperature: " + String(sky_temp) + "C ");

  }

  return sky_temp;
}

float get_cloud_value() {
  // taken from "Detecting cloud with a simple infra-red sensor"
  clear_sky_temp = clear_temp();

  // From https://github.com/dokeeffe/cloud-rain-monitor/blob/master/device/rpi/web/charts.py

  float temp_diff = tempOC - clear_sky_temp;
  float cloud_factor = temp_diff/30;
        if (cloud_factor < 0) {
            return 1;
        }
        else if (cloud_factor > 1) {
            return 0;
        }
        else {
            return 1 - cloud_factor;
        }

}
  
void collecting_data_from_sensors(){
  Serial.println();
  Serial.println("Collecting data: ");
    
  float values[3];
  // Sensor 1 650nm filter filter
  //TCA9548A(2);
  //tsl2591[0].advancedRead(values);
  //data[0] = values[0];        // Lux
  //data[1] = values[1];        // IR
  //data[1] = values[2];        // Full
  //Serial.println("Lux from sensor 1 :" + String(values[0])); 

  // Sensor 2 557 nm filter
  TCA9548A(3);
  tsl2591[1].advancedRead(values);
  //data[3] = values[0];        // Lux
  data[2] = clear_sky_temp; //values[1];        // IR
  data[3] = values[2];        // Full
  Serial.println("Lux from sensor 2 :" + String(values[0]));  

  // Sensor 3 without filter
  TCA9548A(4);
  tsl2591[2].advancedRead(values);
  data[4] = values[0];        // Lux
  data[5] = values[1];        // IR
  data[6] = values[2];        // Full
  Serial.println("Lux from sensor 3 :" + String(values[0])); 

  // Voltage on battery
  raw = analogRead(A0);
  data2[1] = max_voltage*raw/1023;
  Serial.println("Voltage on battery: " + String(data2[1]));

  // Ratio 557nm/Without
  data[7] = data[3]/data[6];

  
  TCA9548A(5);
  float cloud = get_cloud_value();
  data2[0] = cloud;
  Serial.println("Cloud value: " + String(cloud));

  float aurora = auror.get_aurora_points(data[5],data[6],data[3],data[2],cloud, night, _557_weight, fraction_weight);
  Serial.println("Aurora points: " + String(aurora));
  delay(200);
  TCA9548A(5);
  float x;  
  x = mlx.readAmbientTempC();
  x = mlx.readAmbientTempC();
  if (!isnan(x)) { // On success, read() will return 1, on fail 0.
    
    //x = therm.ambient();
    Serial.println("Ambient temperature: " + String(x));
    tempAC = x;
  }
  
  data[0] = tempAC;
  delay(200);
  TCA9548A(5);
  float y; 
  y = mlx.readObjectTempC();
  y = mlx.readObjectTempC();
  if ( !isnan(y)) {
    //y = therm.object();
    
    // Adjusting crasy sensor
    tempOC = y*temp_k + temp_b;
    Serial.println("Sky temperature: " + String(tempOC));
  }
  
  
  data[1] = tempOC;

}




void sleep(int sleepsec) {
  Serial.println("System sleeps for " + String(sleepsec/60) + " minutes");
  for (int i = 0; i < number_of_sensors; i++) {
    TCA9548A(i+2);
    tsl2591[i].sleep();
  }
  thingSpeak.sleep(sleepsec);

}



void setup() {
  
  Serial.begin(9600);
  Serial.println("Setup");
  Serial.println(F("DHT11 set up!"));
  dht.begin();
  Wire.begin(sda, scl);
  TCA9548A(5);
  //therm.begin();
  mlx.begin();
  /*
  if (therm.begin() == false){ // Initialize thermal IR sensor
    Serial.println("Qwiic IR thermometer did not acknowledge! Freezing!");
    while(1);
  }
  Serial.println("Qwiic IR Thermometer did acknowledge.");
  
  therm.setUnit(TEMP_C); // Set the library's units to Farenheit
  therm.setEmissivity(emissivity);
  Serial.println("Emissivity: " + String(therm.readEmissivity()));
  */
  for (int i = 0; i < number_of_sensors; i++) {
    delay(2000);
    TCA9548A(i+2);
    tsl2591[i].configureSensor(9876, 600);
  }
  
  
  
  thingSpeak.initiate(ssid, pass, dataPoints, myWriteAPIKey, myReadAPIKey, Channel_ID, client);
  delay(2000);
  thingSpeak2.initiate(ssid, pass, dataPoints2, myWriteAPIKey_2, myReadAPIKey_2, Channel_ID_2, client);
  pinMode(A0, INPUT);
  delay(2000);
}

void loop() {
  
  delay(5000);
  
  thingSpeak.connect_to_internet();   // Connect to internet and ThingSpeak
  collecting_data_from_sensors();     // Collect data from sensors
  thingSpeak.download(read_data_length, read_field_value, read_data, myReadAPIKey_2, Channel_ID_2);
  /*
  if (emissivity != read_data[0]) {
    emissivity = read_data[0];
    TCA9548A(5);
    mlx.writeEmissivity(emissivity);
    TCA9548A(5);
    Serial.println("Emissivity is changed to: " + String(mlx.readEmissivity()));
  }*/
  
  night = read_data[1];
  temp_k = read_data[2];
  temp_b = read_data[3];
  _557_weight = read_data[4];
  fraction_weight = read_data[5];
  thingSpeak.upload(data, false, 0);            // Upload to ThingSpeak
  thingSpeak2.upload(data2,false,0);
    
  
  /*
  if ( night == 0) {
    Serial.println("Going to sleep");
    sleep(sleepmin * 60);
  }*/
  Serial.println();
 
}
 

