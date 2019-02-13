#include <MIDI.h>

#include <math.h>

#include <SPI.h>

#define oscCount 6

// Created and binds the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();
 
//midi note periods in microseconds
float periods[128] = {
  122312,
  115447,
  108968,
  102852,
  97079,
  91631,
  86488,
  81634,
  77052,
  72727,
  68645,
  64793,
  61156,
  57724,
  54484,
  51426,
  48540,
  45815,
  43244,
  40817,
  38526,
  36364,
  34323,
  32396,
  30578,
  28862,
  27242,
  25713,
  24270,
  22908,
  21622,
  20408,
  19263,
  18182,
  17161,
  16198,
  15289,
  14431,
  13621,
  12856,
  12135,
  11454,
  10811,
  10204,
  9631,
  9091,
  8581,
  8099,
  7645,
  7215,
  6810,
  6428,
  6067,
  5727,
  5405,
  5102,
  4816,
  4545,
  4290,
  4050,
  3822,
  3608,
  3405,
  3214,
  3034,
  2863,
  2703,
  2551,
  2408,
  2273,
  2145,
  2025,
  1911,
  1804,
  1703,
  1607,
  1517,
  1432,
  1351,
  1276,
  1204,
  1136,
  1073,
  1012,
  956,
  902,
  851,
  804,
  758,
  716,
  676,
  638,
  602,
  568,
  536,
  506,
  478,
  451,
  426,
  402,
  379,
  358,
  338,
  319,
  301,
  284,
  268,
  253,
  239,
  225,
  213,
  201,
  190,
  179,
  169,
  159,
  150,
  142,
  134,
  127,
  119,
  113,
  106,
  100,
  95,
  89,
  84,
  80
};

// setting based on transistor behavior npn/pnp
int charge = HIGH;
int ground = LOW;
int drain = 100;
int resSteps = 100000 / 128;

class Oscillator
{
  // Class Member Variables initialized at startup
  int tranPin;      // the number of the transistor pin
  int csPin;        // the number of the mcp pot chip select pin
  byte csAddress;       // A/B address for mcp pot
  long chargeTime;     // milliseconds of charge-time
  long groundTime;    // milliseconds of ground-time
  int chargeRes;       //charge resistance for mcp pot
 
  // These maintain the current state
  int outState;                 // ledState used to set the tansistor
  unsigned long previousMicros;   // will store last time transistor was updated

  // Note handling
  int active;      // true if note currently playing
  int period;     // pitch that osc is currently playing
   
  // Constructor - creates a Flasher and initializes the member variables and state
  public:
  Oscillator(int tPin, int cPin, int abSide)
  {
  tranPin = tPin;
  csPin = cPin;
  csAddress = abSide;
  pinMode(csPin, OUTPUT);
  pinMode(tranPin, OUTPUT);     
    
  chargeTime = 0;
  groundTime = INFINITY;
  
  outState = ground; 
  previousMicros = 0;

  active = LOW;
  period = INFINITY;
  }
 
  void Ring(){
    if (active){
      // check to see if it's time to change the state of the transistor
      // this is done even if there is no note, for cycle consistency by comparing to INF
      unsigned long currentMicros = micros();
       
      if((outState == charge) && (currentMicros - previousMicros >= chargeTime))
      {
        outState = ground;  // done charging, dump the charge
        previousMicros = currentMicros;  // Remember the time
        digitalWrite(tranPin, outState);  // Update the transistor pin
      }
      else if ((outState == ground) && (currentMicros - previousMicros >= groundTime))
      {
        outState = charge;  // done dumping, start charging
        previousMicros = currentMicros;   // Remember the time
        digitalWrite(tranPin, outState);   // Update the transistor pin
      }
    }
  }

  void SetPeriod(byte pitch){
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

  void TurnOff(){
    outState = ground;
    active = LOW;
  }

  int GetActive(){
    return active;
  }
  
  int GetPeriod(){
    return period;
  }
  
};

// create osc
Oscillator osc_stack[oscCount] = {
  Oscillator(22, 30, B00000000),
  Oscillator(23, 30, B00010000),
  Oscillator(24, 31, B00000000),
  Oscillator(25, 31, B00010000),
  Oscillator(26, 32, B00000000),
  Oscillator(27, 32, B00010000)
};

// set callback handles
void handleNoteOn(byte channel, byte pitch, byte velocity){
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  for (int thisOsc = 0; thisOsc < oscCount; thisOsc++) {
    if (! osc_stack[thisOsc].GetActive()) {
      osc_stack[thisOsc].SetPeriod(pitch);
      break;
    }
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity){
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  for (int thisOsc = 0; thisOsc < oscCount; thisOsc++) {
    if (osc_stack[thisOsc].GetPeriod() == periods[int(pitch)]){
      osc_stack[thisOsc].TurnOff();
    }
  }
}

void setup() {
  //midi debug
  pinMode(2, OUTPUT);
  //midi setup
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  MIDI.read();
  for (int thisOsc = 0; thisOsc < oscCount; thisOsc++) {
    osc_stack[thisOsc].Ring();
  }
}
