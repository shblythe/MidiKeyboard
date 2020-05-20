#include <Arduino.h>
#include "Control.h"

const byte Controllers::MidiControlDefaultValues[]=
  { 0,0,127,0,127,0,2,100,64,0,64,127,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,0,
    127,0,0,127,64,0,64,127,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,64,64,64,64,64,
    0,64,64,64,64,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0 };

void Controllers::setup()
{
  for (int i=0; i<MAX_MIDI_CONTROL; i++)
    mControls[i]=new MidiControl(i,MidiControlDefaultValues[i]);
}
