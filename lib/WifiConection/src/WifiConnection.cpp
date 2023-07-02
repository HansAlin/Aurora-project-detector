#include "WiFiConnection.h"

WiFiConnection::WiFiConnection(const char* ssid, const char* password)
  : ssid(ssid), password(password), server(80), longitude(0), latitude(0), zenit(0), utc_off(0), cloud_value_scale(0), temperature(0), humidity(0), aurora_point(0) {}

void WiFiConnection::update() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("Access Point Mode IP address: ");
  Serial.println(WiFi.softAPIP());
  // if (WiFi.status() !=WL_CONNECTED) {
  //   int count = 0;
  //   Serial.println("Trying to access wifi");
  //   while (WiFi.begin() != WL_CONNECTED && count < 10 ) {
  //     Serial.print(".");
  //     count += 1;
  //     delay(500);
  //   }
  // }
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
    html += "<p>Time: " + formatedTime + "</p>";
    html += "<p>Day or Night: " + dayOrNight + "</p>";




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
    ssid = ssidParam.c_str();
    password = passwordParam.c_str();
    Serial.println(ssid);
    Serial.println(password);
    WiFi.begin(ssid, password);
    Serial.print("Local IPaddress: ");
    Serial.println(WiFi.localIP());
    server.send(200, "text/plain", "Wi-Fi connected!");
    delay(1000);
    //ESP.reset();
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
  formatedTime = dataList[3];
  dayOrNight = dataList[4];
}

void WiFiConnection::getAPI(String * dataList) {
  dataList[0] = channel_ID;
  dataList[1] = writeAPI;
  dataList[2] = readAPI; 
}
