#include <Arduino.h>     // Needed when working Platform io
// ----------(c) Electronics-project-hub-------- //
//        Program for ESP8266 D1 MINI
//        
// Libraries
#include <ESP8266WiFi.h>
// #include <Wire.h>
// #include <Adafruit_I2CDevice.h>
// #include <Adafruit_Sensor.h>
// #include "Adafruit_TSL2591.h"
// #include "SPI.h"

// Libraries for this project
#include "AuroraPoints.h"
#include "TSpeak.h" 
#include "TSL2591.h"
#include "CloudCover.h"

#define MLX90614_I2CADDR 0x5A
#define DHTPIN 2 
#define TSL2591_I2CADDR 0x29  // I2c address to light sensor
//------- WI-FI details ----------//
const char * ssid = "ASUS_50_2G"; //      
const char * pass = "jugenjon"; //    
//--------------------------------// 

//----------- Channel settings      ----------------//
// Channel most for writing to ThingSpeak
unsigned long Channel_ID = 1636584; //   collecting data
const char  * myWriteAPIKey = "LA81E3TBW06I54EY"; //
const char  * myReadAPIKey = "L6WSL9CU11VHP4NF";   //
//-------------------------------------------//


// Write to ThingSpeak
int write_fields[] = {1,2,3,4};
const int write_data_length = 4;
float write_data[write_data_length];

//  Reading from ThingSpeak
int read_fields[] = {5,6,7,8}; // fields thats is recived from ThingSpeak
const int read_data_length = 4;
float read_data[read_data_length];

int delayTime = 0;            // Initial delay time for microcontroler 
int sda = 4;                  // Pin on D1 MINI D2 on board ESP8266
int scl = 5;                  // Pin on D1 MINI D1 on board ESP8266
float sleeping;               // If module going to sleep
int SLEEPSEC = 22;            // Short sleep during normal running
int time_to_dusk;             // Time from ThingSpeak in minutes to dusk
unsigned int raw;             // Reading value from A0 analog pin
float max_voltage = 4.1;      // Max voltage on battery
const int number_of_sensors = 2;
int night = 0;                // Returning night veto value from thingSpeak 0=day 1=night

float weight_557 = 1.75;             // Weighting factor to aurora points
float weight_fraction = 150;        // Weighting factor to aurora points       

float clear_sky_temp;
float cloud_value_scale = 30;  // Scaling the output from get_cloud_value 20 - 40 should work

// TODO Make a temperature sensitive sleeping time, colder longer 
// sleepingtime during operation.

WiFiClient  client;

TSL2591 tsl2591[number_of_sensors] = {TSL2591(), TSL2591()} ;
CloudCover cc(DHTPIN);
TSpeak thingSpeak;
AuroraPoints auror;

// Multiplexer
void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}

  
void collecting_data_from_sensors(){
  Serial.println();
  Serial.println("Collecting data: ");
    
  float values[3];  // Array to collecting data from TSL2591 sensors
 
  // Sensor 3 without filter named on container
  Serial.println("Values from sensor without filter (sensor 3) ");
  TCA9548A(4);
  tsl2591[0].advancedRead(values);
  float lux = values[0];    // is Lux from sensor
  float IR = values[1];     // is the IR value from sensor 
  float full = values[2];   // is the full value from senor 
   // Writes in previous operation
  // Serial.println("Lux from sensor 3 :" + String(lux)); 
  // Serial.println("IR from sensor 3 :" + String(IR));
  // Serial.println("Full from sensor 3 :" + String(full)); 

  // Sensor 2 with filter named on container
  Serial.println("Values from sensor with filter (sensor 2) ");
  TCA9548A(3);
  tsl2591[1].advancedRead(values);
  float lux_557 = values[0];        // is Lux from sensor
  float IR_557 =  values[1];        //is the IR value from sensor 
  float full_557 = values[2];       // is the full value from senor
  // Writes in previous operation
  // Serial.println("Lux from sensor 2 :" + String(lux_557)); 
  // Serial.println("IR from sensor 2 :" + String(IR_557));
  // Serial.println("Full from sensor 2 :" + String(full_557)); 
  

  // Voltage on battery
  raw = analogRead(A0);
  float voltage = max_voltage*raw/1023;
  Serial.println("Voltage on battery: " + String(voltage));
  
  TCA9548A(5);
  float cloud = cc.get_cloud_value(cloud_value_scale);
  float temp_at_sensor = cc.get_sensor_temp();
  float humidity = cc.get_humidty();
  Serial.println("Cloud value: " + String(cloud));
  Serial.println("Temerature at detector: " + String(temp_at_sensor));
  Serial.println("Humidity value: " + String(humidity));

  float aurora_point = auror.get_aurora_points(IR, full, full_557, cloud, night, weight_557, weight_fraction );
  Serial.println("Aurora points: " + String(aurora_point));
  
  // Data to ThingSpeak
  write_data[0] = full_557;
  Serial.println("Data to ThingSpeak field number: 1 and data " + String(full_557));    
  write_data[1] = full;
  Serial.println("Data to ThingSpeak field number: 2 and data " + String(full)); 
  // TODO change back to voltage 
  write_data[2] = humidity; 
  Serial.println("Data to ThingSpeak field number: 3 and data " + String(humidity)); 
   
  write_data[3] = aurora_point;
  Serial.println("Data to ThingSpeak field number: 4 and data " + String(aurora_point));  
}




void sleep(int sleepsec) {
  Serial.println("System sleeps for " + String(sleepsec/60) + " minutes");
  
  TCA9548A(4);
  tsl2591[0].sleep();
  TCA9548A(3);
  tsl2591[1].sleep();
  TCA9548A(5);
  cc.sleep();

  thingSpeak.sleep(sleepsec);

  // TCA9548A(3);
  // tsl2591[1].awake();
  // TCA9548A(4);
  // tsl2591[0].awake();

  //TODO implement deepsleep for MLX90614
}



void setup() {
  Serial.begin(9600);
  Serial.println("Set-up!");
  delay(50);
  Wire.begin(sda, scl);
  
  
  
  delay(50);
  Serial.println("Set-up sensor 3!");
  TCA9548A(4);
  //
  tsl2591[0].begin(Wire, TSL2591_I2CADDR);
  tsl2591[0].configureSensor(9876, 600);
  delay(50);

  Serial.println("Set-up sensor 2!");
  TCA9548A(3);
  //
  tsl2591[1].begin(Wire, TSL2591_I2CADDR);
  tsl2591[1].configureSensor(9876, 600);
  delay(50);

  Serial.println("Set-up DHT and MLX!");
  TCA9548A(5);
  //Wire, MLX90614_I2CADDR
  cc.begin(sda, scl);
  
  thingSpeak.initiate(ssid, pass, myWriteAPIKey, myReadAPIKey, Channel_ID, client);
  delay(50);
  pinMode(A0, INPUT);
  delay(50);
}

void loop() {
  
  delay(50);
  
  thingSpeak.connect_to_internet();   // Connect to internet and ThingSpeak
  collecting_data_from_sensors();     // Collect data from sensors
  thingSpeak.download(read_data_length, read_fields, read_data);
 
  time_to_dusk = read_data[0];
  cloud_value_scale = read_data[1];
  weight_557 = read_data[2];
  weight_fraction = read_data[3];

  thingSpeak.upload(write_data, write_fields, write_data_length );            // Upload to ThingSpeak
  
  if (time_to_dusk < 0) {
    // Night !
    night = 1;
    sleep(SLEEPSEC);
  }
  else {
    night = 0;
    sleep(time_to_dusk*60);
  }
}
 

