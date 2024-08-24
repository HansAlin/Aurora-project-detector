#include "Arduino.h"
#include "AuroraPoints.h"
#include <cmath>

/**
 * @brief Construct a new Aurora Points:: Aurora Points object
 * 
 */
AuroraPoints::AuroraPoints(float pre_mean) {
  ir = -1;
  full = -1;
  _557 = -1;
  if_first = true;

  // Variables for handling spikes in full data
  spike_full_limit = 300;            // Value above average thats a spike in data
  for (int i = 0; i < len_full_history; i++) {
    full_history[i] = pre_mean; // Initialize all elements of full_history to 0
  }

  // Variables for hanling spikes in 557nm data
  spike_557_limit = 35;            // Value above average thats a spike in data


}

float AuroraPoints::get_aurora_points(float IR, float FULL, float FULL_557, float clear_sky_value, bool night, float weight_557) {
  /**
   * @brief This function calculates the aurora points
   * @param IR  IR value from sensor without filter 
   * @param FULL FULL value from sensor without filter
   * @param FULL_557 Full value from sensor with filter
   * @param clear_sky_value value from a CloudCover object cloud value
   * @param night night veto
   * @param weight_557 weighting factor for FULL_557 value
   * @param raction_weight weighting factor for fraction between   
   * 
   * @return return the aurora points
   */

  float max_full_prev_diff = 1000;  // Experience based value
  float max_557_diff = 35;          // Experience based value
  float scale_factor = 150;         // Experience based value for scaling the points
  float fraction_limit = 0.002;     // Experience based value for limiting the fraction between 557nm and full
  float max_abs_full = 3000;        // Experience based value for limiting the full value
  float max_abs_557 = 50;           // Experience based value for limiting the 557nm value
  float clear_sky_factor = 1.2;     // Experience based value for supressing low values
  float points_total = 0;
  float epsilon = 0.0001;           // Small value 


  if (if_first) {
    full = FULL;
    _557 = FULL_557;
    if_first = false;
    Serial.println("First values: " + String{full} + " " + String{_557});
  }

  // ##### Constraints on full data #####
  // Get rid of artificial values, value differences between previous value and current value
  // should not be more than 1000
  Serial.println("Full value: " + String{FULL} + " " + String{full} + " " + String{FULL - full});
  if ( abs(FULL - full) > max_full_prev_diff ) {
    Serial.println("Full value too high" + String(FULL - full));
    ir = IR;
    full = FULL;
    _557 = FULL_557;
    return -301;

  }

  // If fraction is too low, return 0
  Serial.println("FULL_557: " + String{FULL_557} + " FULL: " + String{FULL});
  if (FULL != 0) {
    if (FULL_557/FULL < fraction_limit) {
      Serial.println("Fraction too low" + String(FULL_557/FULL));
      ir = IR;
      full = FULL;
      _557 = FULL_557;
      return -302;
    }
  }
  
  Serial.println("Full value: " + String{FULL} );
  if (FULL < epsilon ) {
    Serial.println("Full value zero");
    ir = IR;
    full = FULL;
    _557 = FULL_557;
    return -303;
  }
  
  Serial.println("IR value: " + String{IR});
  if (IR < epsilon) {
    Serial.println("IR value zero");
    ir = IR;
    full = FULL;
    _557 = FULL_557;
    return -304;
  }
  
  Serial.println("Night value: " + String{night});
  if (night == false ) {
    Serial.println("Daytime");
    ir = IR;
    full = FULL;
    _557 = FULL_557;
    return -305;
  }

  // Check if there is a spike in the data compared to the average of the last 6 values
  if (if_spike(full_history, len_full_history, spike_full_limit, FULL)) {
    Serial.println("Full spike");
    ir = IR;
    full = FULL;
    _557 = FULL_557;
    return -306;
  }

  // If full are to high
  if (FULL > max_abs_full) {
    Serial.println("Full value too high");
    ir = IR;
    full = FULL;
    _557 = FULL_557;
    return -307;
  }

  // ##### Constraints on 557nm data #####
  // Get rid of spike values, value differences between brevious value and current value
  // should not be more than 35
  if (abs(FULL_557 - _557) > max_557_diff) {
    Serial.println("557nm value differance too high");
    ir = IR;
    full = FULL;
    _557 = FULL_557;
    return -308;
  }

  // 557nm value should not be more than 50
  if (FULL_557 > max_abs_557) {
    Serial.println("557nm value too high");
    ir = IR;
    full = FULL;
    _557 = FULL_557;
    return -309;
  }



 
  // ##### Calculate aurora points #####
  // Weightning between pure absolute value of 557nm and the relative strength to visible light
  Serial.println("Calculating points");
  float points_557 = _557*weight_557;
  float denominator = FULL - 2*IR;      // Some kind of visible light approximation
  float points_fraction_557_FULL;
  if (denominator <= 0) {
    points_fraction_557_FULL = 0;
    Serial.println("Denominator zero");
  }
  else {
    points_fraction_557_FULL = _557/denominator * scale_factor *(1 - weight_557);
    Serial.println("Denominator not zero");
  }
  // Supress low values
  clear_sky_value = pow(clear_sky_value, clear_sky_factor);
  Serial.println("Clear sky value: " + String{clear_sky_value});
  
    
  points_total = (points_557 + points_fraction_557_FULL) * clear_sky_value;
  // Serial.println("Weighted 557nm value: " + String(points_557));
  // Serial.println("Fraction points: " + String(points_fraction_557_FULL));
  // Serial.println("Cloud cover: " + String(clear_sky_value));
  // Serial.println("Total points " + String(points_total));

  Serial.println("Points: " + String{points_total});
  // Update values for next iteration
  ir = IR;
  full = FULL;
  _557 = FULL_557;

  return points_total; 
}

bool AuroraPoints::if_spike(int * history, int len_history, float spike_limit, float value) {
  float mean = 0;
  bool spike = true;
  float sum = 0;

  for (int i = 0; i < len_history; i++) {
    sum += history[i];
  } 
  mean = sum/len_history;
  
  if ((value ) > mean + spike_limit) {
    spike = true;
    Serial.println("SPIKE!!!");
  }
  else {
    spike = false;
  }
  for (int j = 0; j < len_history; j++) {
      history[j] = history[j + 1];
    }

  Serial.println("Average value from " + String{len_history} + " full values: " + String{mean});
  if (spike) {
    // Average out the spike value
    history[len_history - 1] = (mean + value)/2; 
  }
  else {
    history[len_history - 1] = value; 
  }
  

  return spike;
}




