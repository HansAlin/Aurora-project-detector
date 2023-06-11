#include "Arduino.h"
#include "Time.h"

WiFiUDP ntpUDP;
NTPClient timeClient( ntpUDP);

Time::Time() {
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP);
}

void Time::begin() {
  timeClient.begin();
  timeClient.setTimeOffset(UTCoffSet);

}
int Time::getDayOfYear() {
  
}

void Time::setUTCoffSet(int hour_offset) {
  UTCoffSet = hour_offset * 3600;
}

void Time::upDtate() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedTime();
}

int Time::getMinutes() {
  
}
