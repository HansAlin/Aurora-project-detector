#include "Arduino.h"
#include "TimeKeeping.h"

WiFiUDP ntpUDP;
NTPClient timeClient( ntpUDP);

TimeKeeping::TimeKeeping() {
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);
}

void TimeKeeping::begin(int UTC_off) {
  Serial.println("TimeKeeping begins!");
  setUTCoffSet(UTC_off);
  timeClient.begin();
  timeClient.setTimeOffset(UTCoffSet);

}


int TimeKeeping::getDayOfYear() {
  
  int days = epochTime % 31556926; // Days since New Year in seconds
  // TODO remove print
  Serial.println("Year: " + String(days));

  days = days / 86400;
  return days;
}

int TimeKeeping::getMinutes() {

  int minutes = timeClient.getMinutes();
  int hours = timeClient.getHours();

  return minutes + 60*hours;
}

int TimeKeeping::getYear() {
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  int currentYear = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);
  return currentYear;
}

void TimeKeeping::setUTCoffSet(int hour_offset) {
  UTCoffSet = hour_offset * 3600;
}

void TimeKeeping::upDate(int utc_off) {
  
  timeClient.update();
  formatedTime = timeClient.getFormattedTime();
  epochTime = timeClient.getEpochTime();
  setUTCoffSet(utc_off);
  timeClient.setTimeOffset(UTCoffSet);
}

String TimeKeeping::getFormatedTime() {
  return formatedTime;
}



unsigned long TimeKeeping::getEpochTime() {
  return epochTime;
}
