#include <Arduino.h>
#include <MIDI.h>
#include <math.h>
#include <SPI.h>

#include <freq_table.cpp>

#define oscCount 6

// Created and binds the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();
 
// setting based on transistor behavior npn/pnp
int charge = HIGH;
int ground = LOW;
int drain = 100;
int resSteps = 100000 / 128;

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
