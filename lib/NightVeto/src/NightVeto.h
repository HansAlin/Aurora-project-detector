#include "Arduino.h"
#ifndef NightVeto_H
#define NightVeto_H

#define DAYS_IN_YEAR 365
#define ZENIT_ANGLE 102

class NightVeto {
  public:
    NightVeto();
    void init(float longitude, float latitude, float UTCOff);
    bool ifNight(int day_of_year, float now);
    

  private:
    float gamma(int day_of_year);
    float eqTime(float gamma);
    float decl(float gamma);
    float hour_angle(float lat, float decl, float zenit);
    int dayOfYear();
    float calcDawn(float ha, float e, float longitude);
    float calcDusk(float ha, float e, float longitude);
    float toDeg(float rad);
    float toRad(float deg);
    void setLat(float lat);
    float timeConverter(float min, int today, int UTCoff);
    float timeToDusk(float dawn, float dusk, float now);
    float Now();
    String date;
    float g;
    float e;
    float d;
    float ha;
    float _lat;
    float _long;
    float _UTCoff;
    float dawn;
    float dusk;




};
#endif