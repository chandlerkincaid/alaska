#include <oscillator.h>
#include <freq_table.h>

Oscillator::Oscillator(int fpin){
  pinMode(fpin, OUTPUT);     
  long chargeTime = 0;
  long groundTime = INFINITY;
  unsigned long previousMicros = 0;
  int active = LOW;
  int period = INFINITY;
  int charge = HIGH;
  int ground = LOW;
  int outState = ground; 
  int drain = 10;
}

void Oscillator::Ring(){
  if (active){
    // check to see if it's time to change the state of the transistor
    // this is done even if there is no note, for cycle consistency by comparing to INF
    unsigned long currentMicros = micros();
       
    if((outState == charge) && (currentMicros - previousMicros >= chargeTime))
    {
      outState = ground;  // done charging, dump the charge
      previousMicros = currentMicros;  // Remember the time
      digitalWrite(fpin, outState);  // Update the transistor pin
    }
    else if ((outState == ground) && (currentMicros - previousMicros >= groundTime))
    {
      outState = charge;  // done dumping, start charging
      previousMicros = currentMicros;   // Remember the time
      digitalWrite(fpin, outState);   // Update the transistor pin
    }
  }
}

void Oscillator::SetPeriod(byte pitch){
  period = periods[int(pitch)];
  active = HIGH;
  //modify on off frequency
  chargeTime = period - drain;
  groundTime = drain;
  // also have to set RC charge factor
  // microsecond periods and 1uf cap cancel units, 5tau, pot is 100k 128 steps
  //chargeRes = period / 5;
  //digitalWrite(csPin, LOW);
  //SPI.transfer(csAddress);
  //SPI.transfer(constrain(chargeRes / resSteps, 0, 127));
  //digitalWrite(csPin, HIGH);
}

//   void TurnOff(){
//     outState = ground;
//     active = LOW;
//   }

//   int GetActive(){
//     return active;
//   }
  
//   int GetPeriod(){
//     return period;
//   }