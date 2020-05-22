#include <Arduino.h>
#include "Control.h"

Controllers Controllers::instance;

const char Controllers::MidiControlDefaultValues[]=
  { 0,0,127,0,127,0,2,100,64,0,64,127,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,0,
    127,0,0,127,64,0,64,127,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,64,64,64,64,64,
    0,64,64,64,64,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0 };


void Controllers::setup()
{
  for (byte i=0; i<MAX_MIDI_CONTROL; i++)
    mControls[i]=new MidiControl(i,MidiControlDefaultValues[i]);
  mControls[CONTROL_PITCH_BEND]=new PitchBendControl(CONTROL_PITCH_BEND,64);
  mControls[CONTROL_MASTER_VOLUME]=new MasterVolumeControl(CONTROL_MASTER_VOLUME,100);
  mControls[CONTROL_PROGRAM]=new ProgramControl(CONTROL_PROGRAM,0);
  mControls[CONTROL_CHANNEL]=new ValueControl(CONTROL_CHANNEL,1,16,1,2);
  mControls[CONTROL_OCTAVE]=new ValueControl(CONTROL_OCTAVE,0,3,-3,1);
  mControls[CONTROL_TEMPO]=new ValueControl(CONTROL_TEMPO,0,0,0); // Not sure how to do tempo yet!
  mControls[CONTROL_KEYBOARD_CURVE]=new ValueControl(CONTROL_KEYBOARD_CURVE,0,4);
}
