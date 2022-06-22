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

float AuroraPoints::get_aurora_points(float IR, float FULL, float FULL_557, float cloud_cover, int night, float weight_557=1.9, float fraction_weight=125) {
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
  // Get rid of zero values
  if (FULL_557 != 0) {
    _557 = FULL_557;
  }

  float points_557 = _557*weight_557;
  
  float points_fraction_557_FULL = _557/(full - 2*ir) * fraction_weight;
    
  float points_clear_sky = cloud_cover;
  float points_total = (points_557 + points_fraction_557_FULL + points_fraction_557_FULL) * points_clear_sky;
    
  return points_total; 
}




