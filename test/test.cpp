#include "Arduino.h"
#include "AuroraPoints.h"

AuroraPoints auror;

void setup() {
  float value = auror.Calculate_points(0.2);
  Serial.println(value);

}

void loop() {

}
