#include "Arduino.h"

#include <ThingSpeak.h>
#include "TSpeak.h"
#include <ESP8266WiFi.h>



TSpeak::TSpeak() {
}

void TSpeak::initiate(const char * ssid, const char * pass, const char * myWriteAPIKey, const char * myReadAPIKey, unsigned long Channel_ID, WiFiClient  &client ) {
  
  //WiFiClient  client;
  WiFi.mode(WIFI_STA);
  
  
  _ssid = ssid;
  _pass = pass;
  _myWriteAPIKey = myWriteAPIKey; 
  _myReadAPIKey = myReadAPIKey;
  _Channel_ID = Channel_ID;
  _client = client;
}

void TSpeak::connect_to_internet() {
 
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(_ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(_ssid, _pass);  
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
}

void TSpeak::upload(float data[],int * fieldNumber, int dataPoints) {
  
  ThingSpeak.begin(_client);
  Serial.println("Upload data to ThingSpeak");
  
  for(int i = 0; i < dataPoints; i++){
  ThingSpeak.setField((fieldNumber[i]), data[i]);
  }
  
  int x = ThingSpeak.writeFields(_Channel_ID, _myWriteAPIKey);   
  
  if(x  == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(1000);         // Shortest time between updates. Depends on subscription at ThingSpeak
}


void TSpeak::download(int read_data_length, int * fieldNumber, float * data) {
  //WiFiClient  client;
  Serial.println("Downloading " + String(read_data_length) +" number of data from ThingSpeak");
  ThingSpeak.begin(_client);
  
  for (int i = 0; i < read_data_length; i++ ){
    data[i] = ThingSpeak.readFloatField(_Channel_ID, fieldNumber[i], _myReadAPIKey);
    Serial.println("Data from ThingSpeak field number: " + String(fieldNumber[i]) + " and data " + String(data[i]));  
  }
  
}

void TSpeak::disconnect_internet() {
  WiFi.mode(WIFI_OFF);
}

void TSpeak::sleep(int sleepsec) {
  ESP.deepSleep(sleepsec * 1000000); // Adjusting to get 1 min sleep   
}