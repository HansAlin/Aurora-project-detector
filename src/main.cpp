#include <Arduino.h>     // Needed when working Platform io
// ----------(c) Electronics-project-hub-------- //
//        Program for ESP8266 D1 MINI
//        
// Libraries
#include <ESP8266WiFi.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_MLX90614.h>
#include "SPI.h"
#include <Adafruit_Sensor.h>

// Libraries for this project
#include "AuroraPoints.h"
#include "TSpeak.h" 
#include "TSL2591.h"
#include "CloudCover.h"
#include "WifiConnection.h"
//#include "SimpleDHT.h"
#include "DHT.h"

#define MLX90614_I2CADDR 0x5A
#define DHTPIN D7              // D4 on wemos d1 mini
#define DHTTYPE DHT22
#define TSL2591_I2CADDR 0x29  // I2c address to light sensor
#define MAX_SLEPING_TIME 200 // Sleeping time in minutes 
#define SENSOR_READ_RETRIES 5  // How many atempt to read mlx sensor
#define SENSOR_READ_CYCLES 5  // How often certain sensors should be readed

//------- WI-FI details ----------//
const char * ssid = "ASUS_50_2G" ; //     
const char * pass = "jugenjon"; //    
//--------------------------------// 

//----------- Channel settings      ----------------//
// Channel most for writing to ThingSpeak
unsigned long Channel_ID  = 1636584; //  collecting data;
const char  *myWriteAPIKey = "LA81E3TBW06I54EY"; // 
const char *myReadAPIKey = "L6WSL9CU11VHP4NF";   // 
//-------------------------------------------//

// TODO remove after testing
//----------- Test Channel settings      ----------------//
// Channel most for writing to ThingSpeak
unsigned long Channel_ID_2  = 1669919; //  collecting data;
const char  *myWriteAPIKey_2 = "7RWCPY9QU55V9UZS"; // 
const char *myReadAPIKey_2 = "LYRSAAYO3E6TRSIB";   // 
//-------------------------------------------//

// Write to ThingSpeak
int write_fields[] = {1,2,3,4};
const int write_data_length = 4;
float write_data[write_data_length];

// TODO remove after testing
int test_write_fields[] = {1,2,3,4,5,6,7,8};
const int test_write_data_length = 8;
float test_write_data[test_write_data_length];

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

float weight_557 = 1;         // Weighting factor between raw 557nm data and aurora points
                              // Values 0 to 1. 1 corresponds to that only raw data from 557nm filter sensor is taking 
                              // into acount, 0 corresponds to only calculated points contributes to aurora points     

float clear_sky_temp;
float cloud_value_scale = 0.6;   // Scaling the output from get_cloud_value, values 0 to 1 where. 0.6 is good 
                               // starting point, if cloud values are to hight raise value and vice versa
float object_temp_adjust = 0;  // Scaling the interference from ambient temperature on sky temperure
                               // values 0 to 1, 0.1 measn subtracting 10 % of the value acounted 
                               // from the ambient temperature
float temperature = 0;
float humidity = 0;  
float ambientTemp = 999;
float objectTemp = 999;
int count = 1;                // Take care of how many times data is looked up                             

// TODO Make a temperature sensitive sleeping time, colder longer 
// sleepingtime during operation.
// TODO use wifimanager for  api and channel
// TODO Try to get the DHT22 work inside CloudCover class
// TODO Maybe don't look up the cloud value every time

WiFiClient  client;

TSL2591 tsl2591_1 = TSL2591();
TSL2591 tsl2591_2 = TSL2591();
CloudCover cc;
TSpeak thingSpeak;
// TODO thingSpeak2 is just testing 
TSpeak thingSpeak2;
AuroraPoints auror;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

DHT dht(DHTPIN, DHTTYPE);
// WifiConnection wf;

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
 
  // Sensor 1 without filter named on container
  Serial.println("Values from sensor without filter (sensor 1) ");
  TCA9548A(2);
  tsl2591_1.advancedRead(values);
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
  tsl2591_2.advancedRead(values);
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
  
  // Only check humidty and every 5:th 
  // TODO remove print
  Serial.println("Count " + String(count)) ;
  // Read temp and humidity from DHT22
  if (count == SENSOR_READ_CYCLES) {
    float return_value;
    return_value = dht.readHumidity();
    if (!isnan(return_value)) {
      Serial.println(F("Manage to read from DHT sensor!"));
      humidity = return_value;
    }
    else {
      Serial.println("Did not manage to read humidity");
    }
    delay(2000);
    return_value= dht.readTemperature();
    if (!isnan(return_value)) {
      Serial.println(F("Manage to read from DHT sensor!"));
      temperature = return_value;
    }
    else {
      Serial.println("Did not manage to read temperature!");
    }
    
  }
  // Read object temp and ambient temp from MLX
  if (count == SENSOR_READ_CYCLES) {
    TCA9548A(4);
    float x = mlx.readAmbientTempC();
    int counter = 1;
    while (((x < -40.0) || (x > 125.0)) && counter <= SENSOR_READ_RETRIES) { // On success, read() will return 1, on fail 0.
      x = mlx.readAmbientTempC();
      counter++;
      
    }
    if ((x > -40.0) & (x < 125.0) & !isnan(x)) {
      Serial.println("Manage to read from MLX! ");
      Serial.println("Ambient temperature: " + String(x));
      ambientTemp = x;  // Not used
    }
    
    x= mlx.readObjectTempC();
    counter = 1;
    while (((x < -70.0) || (x > 380.0)) && counter <= SENSOR_READ_RETRIES) {
      x = mlx.readObjectTempC();
      counter++;
    }
    if ( !isnan(x) &  x < 380.0 & x > -70.0) {
      
      objectTemp = x;
      Serial.println("Manage to read from MLX! ");
      Serial.println("Sky temperature: " + String(objectTemp));
      objectTemp = objectTemp + 273;
      objectTemp = pow(pow(objectTemp, 4)/object_temp_adjust, 0.25);
      objectTemp = objectTemp - 273;
      Serial.println("Adjusted sky temperature: " + String(objectTemp));
    }
    count = 0;
  }
  float cloud = cc.get_cloud_value(cloud_value_scale, object_temp_adjust, humidity, temperature, objectTemp, ambientTemp);
  Serial.println("Cloud value: " + String(cloud));
  Serial.println("Temerature at detector: " + String(temperature));
  Serial.println("Humidity value: " + String(humidity));
  Serial.println("Ambient temperature at detector: " + String(ambientTemp));
  Serial.println("Object temperature / sky temperature: " + String(objectTemp));

  float aurora_point = auror.get_aurora_points(IR, full, full_557, cloud, night, weight_557);
  Serial.println("Aurora points: " + String(aurora_point));
  
  // Data to ThingSpeak
  write_data[0] = full_557;
  Serial.println("Data to ThingSpeak field number: 1 and data " + String(full_557));    
  write_data[1] = full;
  Serial.println("Data to ThingSpeak field number: 2 and data " + String(full)); 
  // TODO change back to voltage 
  write_data[2] = cloud; 
  Serial.println("Data to ThingSpeak field number: 3 and data " + String(cloud)); 
   
  write_data[3] = aurora_point;
  Serial.println("Data to ThingSpeak field number: 4 and data " + String(aurora_point));
  
  // TODO remove after testing
  test_write_data[0] = temperature;
  test_write_data[1] = humidity;
  test_write_data[2] = objectTemp;
  test_write_data[3] = ambientTemp;
  test_write_data[4] = IR;
  test_write_data[5] = lux;
  test_write_data[6] = IR_557;
  test_write_data[7] = cc.get_calculated_sky_temp();
  count++;
}

void sleep(int sleepsec) {
  Serial.println("System sleeps for " + String(sleepsec/60) + " minutes");
  
  TCA9548A(2);
  tsl2591_1.sleep();
  TCA9548A(3);
  tsl2591_2.sleep();
  TCA9548A(4);
  cc.sleep();

  thingSpeak.sleep(sleepsec);

  TCA9548A(2);
  tsl2591_1.awake();
  TCA9548A(3);
  tsl2591_2.awake();
  TCA9548A(4);
  cc.wake();

}

void setup() {
  Serial.begin(9600);
  delay(5000);
  Serial.println("Set-up!");
  dht.begin();
  delay(50);
  Wire.begin(sda, scl);
  
  delay(50);
  Serial.println("Set-up sensor 1!");
  TCA9548A(2);
  //
  tsl2591_1.begin();
  tsl2591_1.configureSensor(9876, 600);
  delay(50);

  Serial.println("Set-up sensor 2!");
  TCA9548A(3);
  //
  tsl2591_2.begin();
  tsl2591_2.configureSensor(9876, 600);
  delay(50);

  Serial.println("Set-up DHT and MLX!");
  TCA9548A(4);
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  }
  Serial.println("Emissivity " + String(mlx.readEmissivity()));
  // &myWriteAPIKey, &myReadAPIKey, Channel_ID
  // wf.connect_to_internet();
  Serial.println(myWriteAPIKey);
  Serial.println(myReadAPIKey);
  Serial.println(Channel_ID);
  thingSpeak.initiate(ssid, pass, myWriteAPIKey, myReadAPIKey, Channel_ID, client);
  // TODO remove after testing
  thingSpeak2.initiate(ssid, pass, myWriteAPIKey_2,myReadAPIKey_2, Channel_ID_2, client);
  
  delay(50);
  pinMode(A0, INPUT);
  delay(50);

}

void loop() {
  
  delay(50);
  collecting_data_from_sensors();     // Collect data from sensors
  thingSpeak.connect_to_internet();   // Connect to internet and ThingSpeak
  // TODO remove after test
  thingSpeak2.connect_to_internet();
  thingSpeak2.upload(test_write_data, test_write_fields, test_write_data_length);

  thingSpeak.upload(write_data, write_fields, write_data_length );            // Upload to ThingSpeak
  thingSpeak.download(read_data_length, read_fields, read_data);
  

  night = read_data[0];
  cloud_value_scale = read_data[1];
  weight_557 = read_data[2];
  object_temp_adjust = read_data[3];
  

  delay(5000);
  // // if (time_to_dusk < 0) {
  // //   // Night !
  // //   night = 1;
  // //   sleep(SLEEPSEC);
  // // }
  // // else {
  // //   night = 0;
  // //   if (time_to_dusk > MAX_SLEPING_TIME) {
  // //     sleep(MAX_SLEPING_TIME);
  // //   }
  // //   else {
  // //     sleep(time_to_dusk*60);
  // //   }
    
  // // }
}
 

