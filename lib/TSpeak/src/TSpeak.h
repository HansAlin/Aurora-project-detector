#include "Arduino.h"
#ifndef TSpeak_H
#define TSpeak_H
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <WiFiManager.h>



class TSpeak{
  
  public:
    TSpeak();
    void initiate(const char *WriteAPI, const char *ReadAPI, unsigned long Channel_ID, WiFiClient  &client );
    void updateAPI(const char *WriteAPI, const char *ReadAPI, unsigned long Channel_ID);
    void connect_to_internet();
    void upload(float data[], int * fieldNumber, int dataPoints);
    void download(int read_data_length, int * fieldNumber, float * data);
    void disconnect_internet();
    void sleep(int sleepmin);
  
  private:
    // const char *_ssid;
    // const char *_pass; 
    int _dataPoints;
    unsigned long _Channel_ID;
    const char *_myWriteAPIKey; 
    const char *_myReadAPIKey;
    WiFiClient _client;
    
    
    
};
#endif