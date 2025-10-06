#include "WiFiConnection.h"

static String savedSSID;
static String savedPassword;

WiFiConnection::WiFiConnection(const char* ssid, const char* password)
  : ssid(ssid), password(password), server(80), longitude(0), latitude(0), zenit(0), utc_off(0), cloud_value_scale(0), temperature(0), humidity(0), aurora_point(0) {}

void WiFiConnection::update() {
    Serial.println();
    Serial.println("Starting Wi-Fi connection...");

    WiFi.mode(WIFI_STA);

    // Try to connect using stored credentials first
    WiFi.begin();
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 3000) {
        Serial.print(".");
        delay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("Reconnected to saved Wi-Fi! IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println();
        Serial.println("Failed to connect to saved Wi-Fi. Starting Access Point...");
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP("AuroraSetup", "12345678");  // simple setup AP
        Serial.print("AP Mode IP: ");
        Serial.println(WiFi.softAPIP());
    }
  server.on("/", [this]() {
    String html = "<html><body>";
    html += "<h2>Values</h2>";
    // html += "<p>Longitude: " + String(longitude) + "</p>";
    // html += "<p>Latitude: " + String(latitude) + "</p>";
    // html += "<p>Zenit: " + String(zenit) +  "</p>";
    // html += "<p>UTC off: " + String(utc_off) + "</p>";
    // html += "<p>Cloud value scale: " + String(cloud_value_scale) +  "</p>";
    // html += "<p>Channel ID: " + channel_ID +  "</p>";
    // html += "<p>Write API: " + writeAPI +  "</p>";
    // html += "<p>Read API: " + readAPI +  "</p>";

    html += "<form method='get' action='/update'>";
    html += "<label>Change Longitude:</label>";
    html += "<input type='text' name='longitude' value='" + String(longitude) + "'>";
    html += "<input type='submit' value='Submit'><br>";
    html += "</form>";

    html += "<form method='get' action='/update'>";
    html += "<label>Change Latitude:</label>";
    html += "<input type='text' name='latitude' value='" + String(latitude) + "'>";
    html += "<input type='submit' value='Submit'><br>";
    html += "</form>";

    html += "<form method='get' action='/update'>";
    html += "<label>Change Zenit:</label>";
    html += "<input type='text' name='zenit' value='" + String(zenit) + "'>";
    html += "<input type='submit' value='Submit'><br>";
    html += "</form>";

    html += "<form method='get' action='/update'>";
    html += "<label>Change UTC off:</label>";
    html += "<input type='text' name='utc_off' value='" + String(utc_off) + "'>";
    html += "<input type='submit' value='Submit'><br>";
    html += "</form>";

    html += "<form method='get' action='/update'>";
    html += "<label>Change Cloud Value Scale:</label>";
    html += "<input type='text' name='cloud_value_scale' value='" + String(cloud_value_scale) + "'>";
    html += "<input type='submit' value='Submit'><br>";
    html += "</form>";

    html += "<form method='get' action='/update'>";
    html += "<label>Change Channel ID:</label>";
    html += "<input type='text' name='channel_id' value='" + channel_ID + "'>";
    html += "<input type='submit' value='Submit'><br>";
    html += "</form>";

    html += "<form method='get' action='/update'>";
    html += "<label>Change write API:</label>";
    html += "<input type='text' name='write_API' value='" + writeAPI + "'>";
    html += "<input type='submit' value='Submit'><br>";
    html += "</form>";

    html += "<form method='get' action='/update'>";
    html += "<label>Change read API:</label>";
    html += "<input type='text' name='read_API' value='" + readAPI + "'>";
    html += "<input type='submit' value='Submit'><br>";
    html += "</form>";

    html += "<p>Temperature: " + String(temperature) +  "</p>";
    html += "<p>Humidity: " + String(humidity) +  "</p>";
    html += "<p>Aurora points: " + String(aurora_point) +  "</p>";




    html += "<h1>Wi-Fi Connection</h1>";
    html += "<form method='get' action='/connect'>";
    html += "<label>SSID:</label>";
    html += "<input type='text' name='ssid'><br>";
    html += "<label>Password:</label>";
    html += "<input type='password' name='password'><br>";
    html += "<input type='submit' value='Connect'>";
    html += "</form>";

    
    html += "</body></html>";


    server.send(200, "text/html", html);
  });

  server.on("/update", [this]() {
    if (server.hasArg("longitude")) {
      longitude = server.arg("longitude").toFloat();
    }
    if (server.hasArg("latitude")) {
      latitude = server.arg("latitude").toFloat();
    }
    if (server.hasArg("zenit")) {
      zenit = server.arg("zenit").toFloat();
    }
    if (server.hasArg("utc_off")) {
      utc_off = server.arg("utc_off").toFloat();
    }
    if (server.hasArg("cloud_value_scale")) {
      cloud_value_scale = server.arg("cloud_value_scale").toFloat();
    }
    if (server.hasArg("channel_id")) {
      channel_ID = server.arg("channel_id");
    }
    if (server.hasArg("write_API")) {
      writeAPI = server.arg("write_API");
    }
    if (server.hasArg("read_API")) {
      readAPI = server.arg("read_API");
    }



    server.send(200, "text/plain", "Values updated");
  });

  server.on("/connect", [this]() {
      String ssidParam = server.arg("ssid");
      String passwordParam = server.arg("password");

      Serial.print("Connecting to new Wi-Fi: ");
      Serial.println(ssidParam);

      WiFi.mode(WIFI_STA);
      WiFi.persistent(true);  // save credentials to flash
      WiFi.begin(ssidParam.c_str(), passwordParam.c_str());

      unsigned long startTime = millis();
      bool connected = false;

      // Attempt connection for up to 5 seconds (non-blocking friendly)
      while (millis() - startTime < 5000) {
          if (WiFi.status() == WL_CONNECTED) {
              connected = true;
              break;
          }
          delay(250);
      }

      if (connected) {
          Serial.println();
          Serial.print("Connected! IP: ");
          Serial.println(WiFi.localIP());
          server.send(200, "text/plain", "Wi-Fi connected and saved!");
          delay(500);
          ESP.restart();  // reboot into STA mode with saved credentials
      } else {
          Serial.println("Connection failed!");
          server.send(200, "text/plain", "Connection failed! Check SSID/password.");
      }
  });




  

  server.begin();
  Serial.println("Web server started");
}

// void WiFiConnection::connect() {

// }

void WiFiConnection::handleClient() {
  server.handleClient();
}

void WiFiConnection::getParam(float * data) {
  data[0] = longitude;
  data[1] = latitude;
  data[2] = zenit;
  data[3] = utc_off;
  data[4] = cloud_value_scale;
  
   
}

void WiFiConnection::setParam(float * data) {
  longitude = data[0];
  latitude = data[1];
  zenit = data[2];
  utc_off = data[3];
  cloud_value_scale = data[4];
  
   
}

void WiFiConnection::setData(float * data) {
  full_557 = data[0];
  full = data[1];
  cloud = data[2];
  aurora_point = data[3];
  temperature = data[4];
  humidity = data[5];
  objectTemp = data[6];
}



void WiFiConnection::setAPI(String * dataList) {
  channel_ID = dataList[0];
  writeAPI = dataList[1];
  readAPI = dataList[2];
}

void WiFiConnection::getAPI(String * dataList) {
  dataList[0] = channel_ID;
  dataList[1] = writeAPI;
  dataList[2] = readAPI; 
}
