#include "Arduino.h"
#include "AuroraPoints.h"

/**
 * @brief Construct a new Aurora Points:: Aurora Points object
 * 
 */
AuroraPoints::AuroraPoints() {
  float ir = -1;
  float full = -1;
  float _557 = -1;
}

float AuroraPoints::get_aurora_points(float IR, float FULL, float FULL_557, float cloud_cover, int night, float weight_557=1.0) {
  /**
   * @brief This function calculates the aurora points
   * @param IR  IR value from sensor without filter 
   * @param FULL FULL value from sensor without filter
   * @param FULL_557 Full value from sensor with filter
   * @param cloud_cover value from a CloudCover object cloud value
   * @param night night veto
   * @param weight_557 weighting factor for FULL_557 value
   * @param raction_weight weighting factor for fraction between   
   * 
   * @return return the aurora points
   */
  if (FULL == 0 || IR == 0 || night == 0 ) {
    return 0;
  }
  ir = IR;
  full = FULL;
  // Get rid of spike values
  if (FULL_557 <= 30) {
    _557 = FULL_557;
  }

  float points_557 = _557*weight_557;
  Serial.println("Calculating aurora points");
  float denominator = full - 2*ir;
  float points_fraction_557_FULL;
  if (denominator <= 0) {
    points_fraction_557_FULL = 0;
  }
  else {
    points_fraction_557_FULL = _557/denominator * 150 *(1 - weight_557);
  }
  
  float points_clear_sky = cloud_cover;
  float points_total = (points_557 + points_fraction_557_FULL) * points_clear_sky;
  Serial.println("Weighted 557nm value: " + String(points_557));
  Serial.println("Fraction points: " + String(points_fraction_557_FULL));
  Serial.println("Cloud cover: " + String(cloud_cover));
  Serial.println("Total points " + String(points_total));
  return points_total; 
}




