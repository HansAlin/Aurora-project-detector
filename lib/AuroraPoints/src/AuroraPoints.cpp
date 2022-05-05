#include "Arduino.h"
#include "AuroraPoints.h"






AuroraPoints::AuroraPoints() {
  int average_range = 7;
  float ir[average_range] = {0, 0, 0, 0, 0, 0, 0};
  float full[average_range] = {0, 0, 0, 0, 0, 0, 0};
  float _557[average_range] = {0, 0, 0, 0, 0, 0, 0};
  float _557IR[average_range] = {0, 0, 0, 0, 0, 0, 0};
  
  
}

float AuroraPoints::get_aurora_points(float IR, float FULL, float FULL_557, float IR_557, float cloud_cover, int night, float _557_weight=1.9, float fraction_weight=125) {
  
  if (FULL == 0 || IR == 0 || night == 0) {
    return 0;
  }
  add_value_to_array(IR, & ir);
  add_value_to_array(FULL, & full);
  add_value_to_array(FULL_557, & _557);
  add_value_to_array(IR_557, & _557IR);

  float average_IR = get_average( & ir);
  float average_Full = get_average( & full);
  float average_557 = get_average( & _557);
  float average_557IR = get_average(& _557IR);

  float points_557 = average_557*_557_weight;
  
  float points_fraction_557_FULL = average_557/average_Full * fraction_weight;
    
  float points_clear_sky = cloud_cover;
  float points_total = (points_557 + points_fraction_557_FULL + points_fraction_557_FULL) * points_clear_sky;
    
  return points_total; 
}



void AuroraPoints::add_value_to_array(float value, float array[]) {
  float array_copy[average_range] = {0, 0, 0, 0, 0, 0, 0};

  for(int i = 1; i < average_range; i++ ) {
    array_copy[i-1] = array[i];
  }
  array_copy[average_range-1] = value;

  for (int i = 0; i < average_range; i++) {
    array[i] = array_copy[i];
  }
}

float AuroraPoints::get_average(float array[]) {
  float sum = 0;
  for (int i = 0; i < average_range; i++) {
    sum += array[i];
  }
  float average = sum/average_range;
  return average;
}


