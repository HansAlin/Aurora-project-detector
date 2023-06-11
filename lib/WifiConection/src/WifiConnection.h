#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

class WiFiConnection {
private:
  const char* ssid;
  const char* password;
  ESP8266WebServer server;
  float longitude;
  float latitude;
  float zenit;
  float utc_off;
  float cloud_value_scale;
  float temperature;
  float humidity;
  float aurora_point;
  float full_557;
  float full;
  float cloud;
  float objectTemp;
  String channel_ID;
  String writeAPI;
  String readAPI;
  

public:
  WiFiConnection(const char* ssid, const char* password);
  void update();
  void handleClient();
  void setData(float * data);
  void getParam(float * data);
  void setParam(float * data);
  void setAPI(String * datalist);
  void getAPI(String * datalist);
};

#endif