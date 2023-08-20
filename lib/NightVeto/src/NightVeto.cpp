#include "Arduino.h"
#include "NightVeto.h"

NightVeto::NightVeto() {
  Serial.println("Instantiated NightVeto!");



}
void NightVeto::init(float longitude, float latitude, float UTCOff, float zenit) {
  _long = longitude;
  _lat = latitude;
  _UTCoff = UTCOff;
  _zenit = zenit;

}

bool NightVeto::ifNight(int day_of_year, float now) {

  g = gamma(day_of_year);
  e = eqTime(g);
  d = toDeg(decl(g));
  ha = toDeg(hour_angle(_lat, d, _zenit));
  
  dawn = calcDawn(ha, e, _long);
  dusk = calcDusk(ha, e, _long);

  //TODO remove
  // Serial.println("Ha: " + String(ha));
  // Serial.println("Gamma in rad: " + String(g));
  // Serial.println("EqTime in deg: " + String(e));
  // Serial.println("Declination in deg: " + String(d));
  // Serial.println("Dawn: " + String(dawn));
  // Serial.println("Dusk: " + String(dusk));
  bool veto;
  if ( (dawn < now) & (dusk > now)) {
    veto = false;
  }
  else if (ha == 180) {
    veto = false;
  }
  else {
    veto = true;
  }
    

  return veto ; 
}

float NightVeto::calcDawn(float ha, float e, float longitude) {
  float dawn = 720 - 4*(longitude + ha) - e + _UTCoff*60;
  return dawn;
}

float NightVeto::calcDusk(float ha, float e, float longitude) {
  float dusk = 720 - 4*(longitude - ha) - e + _UTCoff*60;
  return dusk;
}

float NightVeto::Now() {
  return 1;
}

float NightVeto::gamma(int day_of_year) {
  return day_of_year / (float)DAYS_IN_YEAR * 2 * PI;
}

float NightVeto::eqTime(float gamma) {
  return 229.18 * (0.000075 + 0.001868 * cos(gamma) - 0.032077 * sin(gamma) - 0.014615 * cos(2 * gamma) - 0.040849 * sin(2 * gamma));
}

float NightVeto::decl(float gamma) {
  return 0.006918 - 0.399912 * cos(gamma) + 0.070257 * sin(gamma) - 0.006758 * cos(2 * gamma) + 0.000907 * sin(2 * gamma) - 0.002697 * cos(3 * gamma) + 0.00148 * sin(3 * gamma);
}

float NightVeto::hour_angle(float lat, float decl, float zenit) {
  zenit = toRad(zenit);
  decl = toRad(decl);
  lat = toRad(lat);
  float value = cos(zenit)/(cos(lat)*cos(decl)) - tan(lat)*tan(decl);

  if (value >= 1)
    ha = 0;
  else if (value <= -1)
    ha = PI;
  else 
      ha = acos(value);  
  
  return ha;
}

float NightVeto::toRad(float deg) {
  return deg/360 * 2 * PI;
}

float NightVeto::toDeg(float rad) {
  return rad/(2*PI) * 360;
}

float NightVeto::timeConverter(float min, int today, int UTCoff) {
  // TODO This code is not complete
  // loat time = today;
  // time = time + minutes(min);
  // time = time + hours(UTCoff);
  return 0;
}

float NightVeto::timeToDusk(float dawn, float dusk, float now) {
  // TODO this code is note complete
  // if ( dawn > now)
  //   time = minutes(now - dawn);
  //   time = floor(time);
  // elseif ( dawn < now & dusk > now)
  //   time = minutes(dusk - now);
  //   time = floor(time);
  //  else
  //   time = -1;time
  return 0;
}

void NightVeto::setLat(float lat) {
  _lat = lat;
}

int NightVeto::dayOfYear() {
  int days_in_year = 365;
  // DateTime now = ThingSpeak.getTime();
  // int day_of_year = now.dayOfYear();
  return 1;
}

