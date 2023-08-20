#include <Arduino.h>     
#include <ESP8266WiFi.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_MLX90614.h>
#include "SPI.h"
#include <Adafruit_Sensor.h>
#include "DHT.h"

// Libraries for this project
#include "AuroraPoints.h"
#include "TSpeak.h" 
#include "TSL2591.h"
#include "CloudCover.h"
#include "WifiConnection.h"
#include "NightVeto.h"
#include "FileHandle.h"
#include "TimeKeeping.h"


#define MLX90614_I2CADDR 0x5A
#define DHTPIN D7              // D4 on wemos d1 mini
#define DHTTYPE DHT22
#define TSL2591_I2CADDR 0x29  // I2c address to light sensor
#define MAX_SLEPING_TIME 200 // Sleeping time in minutes 
#define SENSOR_READ_RETRIES 5  // How many atempt to read mlx sensor
#define SENSOR_READ_CYCLES 5  // How often certain sensors should be readed


// TODO comment
//------- Set detector credentials ----------//
const char * ssid_detector = "Aurora Detector" ; //     
const char * password_detector = "password"; //    
//--------------------------------// 


//----------- Channel settings      ----------------//
// Channel for writing to ThingSpeak
unsigned long channel_ID_1;
char myWriteAPIKey_1[20]; 
char myReadAPIKey_1[20];
//-------------------------------------------//
const int numOfElementsAPI = 5;
String APIList[numOfElementsAPI] = {String(channel_ID_1), String(myWriteAPIKey_1), String(myReadAPIKey_1), "", ""};


unsigned long input_value;

// Write to ThingSpeak
int write_fields[] = {1,2,3,4,5,6,7,8};
const int write_data_length = 8;
float write_data[write_data_length];


int delayTime = 0;            // Initial delay time for microcontroler 
int sda = 4;                  // Pin on D1 MINI D2 on board ESP8266
int scl = 5;                  // Pin on D1 MINI D1 on board ESP8266
float sleeping;               // If module going to sleep
int SLEEPSEC = 22;            // Short sleep during normal running
int time_to_dusk;             // Time from ThingSpeak in minutes to dusk
unsigned int raw;             // Reading value from A0 analog pin
// float max_voltage = 4.1;      // Max voltage on battery not implemented
const int number_of_sensors = 2;
bool night = 0;                // Returning night veto 

float weight_557 = 1;         // Weighting factor between raw 557nm data and aurora points
                              // Values 0 to 1. 1 corresponds to that only raw data from 557nm filter sensor is taking 
                              // into acount, 0 corresponds to only calculated points contributes to aurora points     

float clear_sky_temp;

float emissivity = 0;  // Scaling the interference from ambient temperature on sky temperure
                               // values 0 to 1, 0.1 measn subtracting 10 % of the value acounted 
                               // from the ambient temperature
float a = 1;                   // Scaling values to sky temp measurment
float b = 0;                    // Temp offset            
float temperature = 0;
float humidity = 0;  
float ambientTemp = 999;
float objectTemp = 999;
int count = 1;                // Take care of how many times data is looked up                             

float lux = 0;                    // is Lux from sensor
float IR = 0;                     // is the IR value from sensor 
float full = 0;                   // is the full value from senor
float lux_557;        // is Lux from sensor
float IR_557;        //is the IR value from sensor 
float full_557;       // is the full value from senor

bool spike;                       // Spike in data
int spike_limit = 300;            // Value above average thats a spike in data
const int len_history = 6;
int full_history[len_history] = {0,0,0,0,0,0}; // keeping track of full values
float aurora_point = 0;
int global_count = 0;



// Initial values, location is Mora Sweden
float longitude = 14.600036;    // Position
float latitude = 61.01030;      // Position
float zenit = 102.0;            // Sun zenit angle
float utc_off = 2.0;            // UTC off cet
float cloud_value_scale = 0.6;  // Scaling the output from get_cloud_value, values 0 to 1 where. 0.6 is good 
                                // starting point, if cloud values are to hight raise value and vice versa

// Data to and from web
const int numOfElementsParamList = 5;                               
float paramList[numOfElementsParamList] = {longitude, latitude, zenit, utc_off, cloud_value_scale};

// Declare functions
void updateParamList(float * data);
void upDateParamFromParamList(float * data);
void upDateAPIList(String * API_data);
void upDateAPIFromList(String * API_data);
void TCA9548A(uint8_t bus);
bool if_spike(int * history, int len_history, float spike_limit, float value);
void collecting_data_from_sensors();
void sleep(int sleepsec);

// TODO Make a temperature sensitive sleeping time, colder longer 
// sleepingtime during operation.

WiFiClient  client;

// Instanciate sensors
TSL2591 tsl2591_1 = TSL2591();
TSL2591 tsl2591_2 = TSL2591();
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
DHT dht(DHTPIN, DHTTYPE);

// Instanciate custom classes
CloudCover cc;
TSpeak thingSpeak;
AuroraPoints auror;
NightVeto nightVeto;
WiFiConnection wifiConnection(ssid_detector, password_detector);
FileHandle fileHandle;
TimeKeeping t;





void setup() {
  Serial.begin(9600);
  delay(5000);
  Serial.println();
  Serial.println("----------  Set-up!  ----------------");
  wifiConnection.update();
  fileHandle.initFS();
  fileHandle.getParam(paramList);         // Get saved data
  fileHandle.getAPI(APIList);           // Get saved data
  upDateParamFromParamList(paramList);   // Update values
  upDateAPIFromList(APIList);           // Update API values
  WiFi.begin();
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
  
  Serial.println(myWriteAPIKey_1);
  Serial.println(myReadAPIKey_1);
  Serial.println(channel_ID_1);
  thingSpeak.initiate(myWriteAPIKey_1, myReadAPIKey_1, channel_ID_1, client);
  
  Serial.println("Set up time: ");
  t.begin(utc_off);
  delay(50);
  pinMode(A0, INPUT);
  delay(50);
  Serial.println("----------  Set-up-done!  ----------------");

}


void loop() {
  Serial.println();
  Serial.println('----------- Loop -------------');
  delay(50);
  t.upDate(utc_off);
  fileHandle.getParam(paramList);
  fileHandle.getAPI(APIList);

  upDateParamFromParamList(paramList);
  upDateAPIFromList(APIList);
  
  wifiConnection.setData(write_data);
  wifiConnection.setParam(paramList);
  wifiConnection.setAPI(APIList);
  wifiConnection.handleClient();
  wifiConnection.getAPI(APIList);
  wifiConnection.getParam(paramList);
  fileHandle.saveParam(paramList);
  fileHandle.saveAPI(APIList);
  upDateParamFromParamList(paramList);
  upDateAPIFromList(APIList);
  upDateAPIList(APIList);
  nightVeto.init(longitude, latitude, utc_off, zenit);

  collecting_data_from_sensors();     // Collect data from sensors
  

  for (int i = 0; i < 8; i++) {
    Serial.print(String(paramList[i]) + " ");
  }
  Serial.println();

  thingSpeak.connect_to_internet();   // Connect to internet and ThingSpeak
  thingSpeak.upload(write_data, write_fields, write_data_length );            // Upload to ThingSpeak

  fileHandle.saveParam(paramList);
  
  Serial.println("---------------");
  Serial.println();
  delay(10000);
 
}

void updateParamList(float * data) {

 
  data[0] = longitude;
  data[1] = latitude;
  data[2] = zenit;
  data[3] = utc_off;
  data[4] = cloud_value_scale;

  Serial.println("Update paramList: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(String(data[i]) + " ");
  }
  Serial.println();
  
}

void upDateParamFromParamList(float * data) {
  longitude = data[0];
  latitude = data[1];
  zenit = data[2];
  utc_off = data[3];
  cloud_value_scale = data[4];

}

void upDateAPIFromList(String * API_data) {
  channel_ID_1 = API_data[0].toInt();

  int str_len = API_data[1].length() + 1;
  myWriteAPIKey_1[str_len];
  API_data[1].toCharArray(myWriteAPIKey_1, str_len);
  
  str_len = API_data[2].length() + 1;
  myReadAPIKey_1[str_len];
  API_data[2].toCharArray(myReadAPIKey_1, str_len);
 

}

void upDateAPIList(String * API_data) {
  API_data[0] = String(channel_ID_1);
  API_data[1] = String(myWriteAPIKey_1);
  API_data[2] = String(myReadAPIKey_1);
  API_data[3] = t.getFormatedTime();
  if (night) {
    API_data[4] = "Night";
  }
  else {
    API_data[4] = "Day";
  }

}

// Multiplexer
void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}

bool if_spike(int * history, int len_history, float spike_limit, float value) {
  float mean = 0;
  bool spike = true;
  float sum = 0;
  int new_history[len_history];
  for (int i = 0; i <= len_history; i++) {
    sum += history[i];
  } 
  mean = sum/len_history;
  
  if ((value ) > mean + spike_limit) {
    spike = true;
    Serial.println("SPIKE!!!");
  }
  else {
    spike = false;
  }
  for (int j = 0; j < len_history; j++) {
      new_history[j] = history[j + 1];
      history[j] = new_history[j];
    }

  Serial.println("Average value from " + String{len_history} + " full values: " + String{mean});
  if (spike) {
    // Average out the spike value
    history[len_history] = (new_history[len_history - 1] + new_history[len_history - 2] + value)/3;
  }
  else {
    history[len_history] = value;
  }
  

  return spike;
}

void collecting_data_from_sensors(){
  Serial.println();
  Serial.println("Collecting data: ");

  night = nightVeto.ifNight(t.getDayOfYear(), t.getMinutes());
    
  float values[3];  // Array to collecting data from TSL2591 sensors
 
  // Sensor 1 without filter named on container
  // Serial.println("Values from sensor without filter (sensor 1) ");
  TCA9548A(2);
  tsl2591_1.advancedRead(values);
  lux = values[0];    // is Lux from sensor
  IR = values[1];     // is the IR value from sensor 
  spike = if_spike( full_history, len_history, spike_limit, values[2]);
  full = values[2];   // is the full value from senor 
  // Writes in previous operation
  // Serial.println("Lux from sensor 3 :" + String(lux)); 
  // Serial.println("IR from sensor 3 :" + String(IR));
  // Serial.println("Full from sensor 3 :" + String(full)); 

  // Sensor 2 with filter named on container
  // Serial.println("Values from sensor with filter (sensor 2) ");
  TCA9548A(3);
  tsl2591_2.advancedRead(values);
  lux_557 = values[0];        // is Lux from sensor
  IR_557 =  values[1];        //is the IR value from sensor 
  full_557 = values[2];       // is the full value from senor
  // Writes in previous operation
  // Serial.println("Lux from sensor 2 :" + String(lux_557)); 
  // Serial.println("IR from sensor 2 :" + String(IR_557));
  // Serial.println("Full from sensor 2 :" + String(full_557)); 
  
  
  // Only check humidty and every 5:th 
  // TODO remove print
  // Serial.println("Count " + String(count)) ;
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
    
    if (!isnan(x)) {
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
    
    if ( !isnan(x) &  x < 380.0 ) { 
      // TODO remove comments
      objectTemp = x;
      // Serial.println("Manage to read from MLX! ");
      // Serial.println("Sky temperature: " + String(objectTemp));
      // objectTemp = objectTemp + 273;
      // objectTemp = pow(pow(objectTemp, 4)/emissivity, 0.25);
      // objectTemp = objectTemp - 273;
     
      // Alternative calculation a = 0.66, b= 105
    
      objectTemp = (objectTemp +273) * a + b;
      objectTemp = objectTemp - 273;
      // One might keep the emission parameter
      //  Serial.println("Adjusted sky temperature: " + String(objectTemp));
    }
    count = 0;
  }
  float cloud = cc.get_cloud_value(cloud_value_scale, humidity, temperature, objectTemp, ambientTemp);
  if (objectTemp < -250) {
    cloud = 0;
  }
  

  float new_aurora_point = auror.get_aurora_points(IR, full, full_557, cloud, night, weight_557);
  // Secure that no temporary high values will be recorded
  if (spike || (new_aurora_point > 50.0) || (full_557/full < 0.002)) {

    Serial.println("Probably not aurora!");
    aurora_point = 0;
  }
  else {
    aurora_point = new_aurora_point; 
  }
  Serial.println("Aurora points                  : " + String(aurora_point));
  Serial.println("Cloud value                    : " + String(cloud));
  Serial.println("Temerature at detector (DHT)   : " + String(temperature));
  Serial.println("Humidity value                 : " + String(humidity));
  Serial.println("Ambient temperature (MLX)      : " + String(ambientTemp));
  Serial.println("Object/sky temperature (MLX)   : " + String(objectTemp));
  Serial.println("Full                           : " + String(full));
  Serial.println("IR                             : " + String(IR));
  Serial.println("Lux                            : " + String(lux));
  Serial.println("Full 557                       : " + String(full_557));
  Serial.println("IR 557                         : " + String(IR_557));
  Serial.println("Lux 557                        : " + String(lux_557));
  Serial.println("Time                           : " + t.getFormatedTime());
  if (night) {
    Serial.println("Day or night                   : Night");  
  }
  else {
    Serial.println("Day or night                   : day");
  }
  Serial.println("Minutes from midnight          : " + String(t.getMinutes()));
  Serial.println("Day of year                    : " + String(t.getDayOfYear()));
  Serial.println("Dusk                           : " + String(nightVeto.dusk));
  Serial.println("Dawn                           : " + String(nightVeto.dawn));
  
  // Data to ThingSpeak
  write_data[0] = full_557;    
  write_data[1] = full;
  write_data[2] = cloud;  
  write_data[3] = aurora_point;
  write_data[4] = temperature;
  write_data[5] = humidity;
  write_data[6] = objectTemp;
  write_data[7] = IR;

  for (int i = 0; i < write_data_length; i++) {
    Serial.println("Data to ThingSpeak field number: " +  String(i+1) + "and data " + String(write_data[i]));
  }
  

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

 