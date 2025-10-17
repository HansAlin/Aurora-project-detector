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
  timeClient.begin();
  timeClient.setTimeOffset(UTC_off*3600);
  Serial.println("Set UTC off-cet: " + String(UTC_off));

}


int TimeKeeping::getDayOfYear() {
  
  int days = epochTime % 31556926; // Days since New Year in seconds
  days = days / 86400;
  // Serial.println("Day  of year: " + String(days));
  return days;
}

int TimeKeeping::getMinutes() {

  int minutes = timeClient.getMinutes();
  int hours = timeClient.getHours();
  minutes = minutes + 60*hours;
  // Serial.println("Present time in minutes: " + String(minutes));
  return minutes;
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
  setUTCoffSet(utc_off);
  timeClient.setTimeOffset(UTCoffSet);
  formatedTime = timeClient.getFormattedTime();
  epochTime = timeClient.getEpochTime();
  
}

String TimeKeeping::getFormatedTime() {
  return formatedTime;
}



unsigned long TimeKeeping::getEpochTime() {
  return epochTime;
}
