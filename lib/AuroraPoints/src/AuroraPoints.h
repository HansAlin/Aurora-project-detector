#include "Arduino.h"
#ifndef AuroraPoints_h
#define AuroraPoints_h

class AuroraPoints {
  public:
    AuroraPoints(float pre_mean);
    float get_aurora_points(float IR, float FULL,  float FULL_557, float clear_sky_value, bool night,  float weight_557);
    float Recive_data();
    
  private:
    void add_value_to_array(float value, float array[]);
    void Setup();
    float get_average(float arrayy[]);
    bool if_spike(int * full_history, int len_full_history, float spike_full_limit, float value);
    int average_range;
    float ir;
    float full;
    float _557;
    float _557IR;
    bool if_first;

    bool full_spike;                       // Spike in data
    int spike_full_limit;            // Value above average thats a spike in data
    static const int len_full_history = 6; // Initialize const variable at declaration
    int full_history[len_full_history];  // Declare full_history as an array of floats

    bool _557_spike;                       // Spike in data
    int spike_557_limit;            // Value above average thats a spike in data

    float aurora_point;
    
};
#endif