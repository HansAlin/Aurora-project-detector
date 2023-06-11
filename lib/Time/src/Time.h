#include "Arduino.h"
#ifndef Time_H
#define Time_H
#include <NTPClient.h>
#include <WiFiUdp.h>



class Time {

  public:
  Time();
  void begin();
  int getDayOfYear();
  int getMinutes();
  void setUTCoffSet(int hour_offset);
  void upDtate();
  

  private:
  String formattedDate;
  String dayStamp;
  String timeStamp;
  int UTCoffSet;

};
#endif