#include "Arduino.h"
#ifndef TimeKeeping_H
#define TimeKeeping_H
#include <NTPClient.h>
#include <WiFiUdp.h>



class TimeKeeping {

  public:
  TimeKeeping();
  void begin(int UTCoff);
  int getDayOfYear();
  int getMinutes();
  int getYear();
  void setUTCoffSet(int hour_offset);
  unsigned long getEpochTime();
  void upDate(int utc_off);
  String getFormatedTime();
  
  

  private:
  
  String formatedTime;
  String dayStamp;
  String timeStamp;
  int UTCoffSet;
  int epochTime;

};
#endif