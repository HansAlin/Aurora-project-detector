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

//#include "CloudCover.h"
#include "AuroraPoints.h"

//#include "Adafruit_GFX.h"
#include "TSpeak.h" 
#include "OPT3001.h"

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
const int dataPoints2 = 3;     // Number of datapoints to send to channel
float data[dataPoints];       // An array with all the datapoints
float data2[dataPoints2];

//int field_average_from_sensor = 1;
int read_field_value[] = {4,5,6,7,8};
const int read_data_length = 5;
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
float temp_adjust;
float cloud_weight;
float _557_weight;
float fraction_weight;
float emissivity = 1;
float tempAC;
float tempOC;

WiFiClient  client;

TSL2591 tsl2591[number_of_sensors] = {TSL2591(), TSL2591(), TSL2591()} ;

TSpeak thingSpeak;
TSpeak thingSpeak2;

Adafruit_MLX90614 mlx;
AuroraPoints auror;


void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}

float get_cloud_value(float temp_adjust=202, float w_1=2) {
  float ambientTemp = tempAC;
  float objectTemp = tempOC;
  float clear_sky_value;
  // To Kelvin
  ambientTemp = ambientTemp + 273;
  objectTemp = objectTemp + 273;

  float tempdiff = ambientTemp - objectTemp;
  clear_sky_value = tempdiff / (ambientTemp - temp_adjust ) * w_1;
  /*if (tempdiff < 0) {
    clear_sky_value = 0;
  }
  else {
    
    if(clear_sky_value > 1) {
      //clear_sky_value = 1;
    }
  }*/
  return clear_sky_value;
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
  data[2] = values[1];        // IR
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
  data2[2] = max_voltage*raw/1023;
  Serial.println("Voltage on battery: " + String(data2[0]));

  // Ratio 557nm/Without
  data[7] = data[3]/data[6];

  
  TCA9548A(5);
  data2[1] = get_cloud_value(temp_adjust,cloud_weight);
  Serial.println("Cloud value: " + String(data2[1]));

  data2[0] = auror.get_aurora_points(data[5],data[6],data[3],data[2],data2[1], night,temp_adjust,cloud_weight, _557_weight, fraction_weight);
  Serial.println("Aurora points: " + String(data2[0]));

  TCA9548A(5);
  float x = mlx.readAmbientTempC();
  x = mlx.readAmbientTempC();
  Serial.println("Ambient temperature: " + String(x));
  if (!isnan(x) && (x < 60) && (x > -60) ) {
    
  }
  tempAC = x;
  data[0] = tempAC;

  TCA9548A(5);
  float y = mlx.readObjectTempC();
  y = mlx.readObjectTempC();
  Serial.println("Sky temperature: " + String(y));
  if (!isnan(y) && (y < 40) && (y > -40) ) {
    
  }
  tempOC = y;
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
  Wire.begin(sda, scl);
  TCA9548A(5);
  mlx.begin();
  TCA9548A(5);
  mlx.writeEmissivity(emissivity);

  
  
  
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
  night = read_data[0];
  temp_adjust = read_data[1];
  cloud_weight = read_data[2];
  _557_weight = read_data[3];
  fraction_weight = read_data[4];
  thingSpeak.upload(data, false, 0);            // Upload to ThingSpeak
  thingSpeak2.upload(data2,false,0);
    
  
  /*
  if ( night == 0) {
    Serial.println("Going to sleep");
    sleep(sleepmin * 60);
  }*/
  Serial.println();
 
}
 

