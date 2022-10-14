#include "Arduino.h"
#include "LittleFS.h"
#include <WiFiManager.h>
#include <ArduinoJson.h>

#ifndef WifiConnection_h
#define WifiConnection_h

class WifiConnection {
  public:
    WifiConnection();
    //String *myWriteAPIKey, String *myReadAPIKey, unsigned long &channel_id
    void connect_to_internet();
    

  private:
    #define ESP_DRD_USE_SPIFFS true  
    String CHANNEL_FILE;
    String WRITE_API_FILE;
    String READ_API_FILE;
    bool shouldSaveConfig;
    unsigned long channel_id;
    String  myWriteAPIKey; //
    String  myReadAPIKey;   //
    bool write_to_file(String file_name, String contents);
    String read_from_file(String path);
    

};
#endif
