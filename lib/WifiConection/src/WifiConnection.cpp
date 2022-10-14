#include "Arduino.h"
#include "WifiConnection.h"
#include "LittleFS.h"
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>



WifiConnection::WifiConnection() {}
// String *myWriteAPIKey, String *myReadAPIKey, unsigned long &channel_id
void WifiConnection::connect_to_internet() {
    
    // CHANNEL_FILE = "/channel_api.txt";
    // WRITE_API_FILE = "/write_api.txt";
    // READ_API_FILE = "/read_api.txt";
    // LittleFS.begin();
    String channel_string = read_from_file(CHANNEL_FILE);
    // channel_id = atol(channel_string.c_str());
    // String WriteAPI = read_from_file(WRITE_API_FILE);
    // myWriteAPIKey = &WriteAPI;
    // String ReadAPI = read_from_file(READ_API_FILE);
    // myReadAPIKey = &ReadAPI;

    bool forceConfig = false;

    if ( channel_string  == "") {
      forceConfig = true;
    }
    
    

    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;
    
    //wm.setSaveConfigCallback(saveConfigCallback);
    //wm.setAPCallback(configModeCallback);

    // wm.resetSettings();
    // WiFiManagerParameter channel_id_text_box("channel_id_text_box", "Channel ID ", "", 50);
    // wm.addParameter(&channel_id_text_box);

    // WiFiManagerParameter write_api_text_box("write_api_text", "API key write: ", "", 50);
    // wm.addParameter(&write_api_text_box);

    // WiFiManagerParameter read_api_text_box("read_api_text_box", "API key read: ", "",50);
    // wm.addParameter(&read_api_text_box);

    if (forceConfig) {
      if (!wm.startConfigPortal("Aurora Detector"))
        {
          Serial.println("failed to connect and hit timeout");
          delay(3000);
          //reset and try again, or maybe put it to deep sleep
          ESP.restart();
          delay(5000);
        }
    }
    else {
      if (!wm.autoConnect("Aurora Detector"))
        {
          Serial.println("failed to connect and hit timeout");
          delay(3000);
          // if we still have not connected restart and try all over again
          ESP.restart();
          delay(5000);
        }
    }
 
  // If we get here, we are connected to the WiFi
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // strncpy(myWriteAPIKey, write_api_text_box.getValue(), sizeof(myWriteAPIKey));
  // strncpy(myReadAPIKey, read_api_text_box.getValue(), sizeof(myReadAPIKey));
  // char *ptr;
  // channel_id = strtoul(channel_id_text_box.getValue(), &ptr, 16);
  // write_to_file(CHANNEL_FILE, channel_id_text_box.getValue() );
  // write_to_file(WRITE_API_FILE, write_api_text_box.getValue() );
  // write_to_file(READ_API_FILE, read_api_text_box.getValue());

  LittleFS.end();
}

String WifiConnection::read_from_file(String file_name) {
    String result = "";
  
  File this_file = LittleFS.open(file_name, "r");
  if (!this_file) { // failed to open the file, retrn empty result
    Serial.println("failed to open the file, return false");
    return result;
  }
  while (this_file.available()) {
      result += (char)this_file.read();
  }
  
  this_file.close();
  return result;
}

bool WifiConnection::write_to_file(String file_name, String contents) {
    File this_file = LittleFS.open(file_name, "w");
  if (!this_file) { // 
    Serial.println("failed to open the file, return false");
    return false;
  }
  int bytesWritten = this_file.print(contents);
 
  if (bytesWritten == 0) { // write failed
      Serial.println("failed to open the file, return false");
      return false;
  }
   
  this_file.close();
  Serial.println("Saved OK return true");
  return true;
}


