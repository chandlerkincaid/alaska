#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <Arduino.h>

class Oscillator{  
public:
    int fpin;      // the number of the transistor pin or raw pwm out
    long chargeTime;      // microseconds of charge-time
    long groundTime;      // microseconds of ground-time
    int outState;     // current value assigned to the freqPin
    unsigned long previousMicros;     // stores last time freqPin was updated
    int active;       // true if note currently playing
    int period;       // pitch that osc is currently playing
    int charge;       // polarity for charging current, particularly for transistor
    int ground;       // polarity for drainging current or ground
    int drain;

    Oscillator(int fpin);
    void Ring();
    void SetPeriod(byte pitch);
    void TurnOff();
};

#endif