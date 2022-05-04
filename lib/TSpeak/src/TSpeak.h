

#include "Arduino.h"
#ifndef TSpeak_H
#define TSpeak_H
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>



class TSpeak{
  
  public:
    TSpeak();
    void initiate(const char * ssid, const char * pass, const int dataPoints, const char * myWriteAPIKey, const char * myReadAPIKey, unsigned long Channel_ID, WiFiClient  &client);
    void connect_to_internet();
    void upload(float data[], bool single, int field);
    void download(int read_data_length, int * FieldNumber, float * data, const char * myReadAPIKey_2, unsigned long Channel_ID_2);
    void disconnect_internet();
    void sleep(int sleepmin);
  
  private:
    const char * _ssid;
    const char * _pass; 
    int _dataPoints;
    unsigned long _Channel_ID;
    const char * _myWriteAPIKey; 
    const char * _myReadAPIKey;
    WiFiClient _client;
    
    
    
};
#endif