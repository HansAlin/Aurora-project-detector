#include "Arduino.h"
#ifndef AuroraPoints_h
#define AuroraPoints_h






class AuroraPoints {
  public:
    AuroraPoints();
    float get_aurora_points(float IR, float FULL,  float IR_557, float cloud_cover, int night,  float _557_weight, float fraction_weight);
    float Recive_data();
    
    

  private:
    void add_value_to_array(float value, float array[]);
    void Setup();
    float get_average(float arrayy[]);
    int average_range;
    float ir;
    float full;
    float _557;
    float _557IR;
    


    
    


};
#endif