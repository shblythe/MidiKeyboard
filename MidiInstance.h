#include <Arduino.h>
#include <MIDI.h>

#ifndef __MIDIINSTANCE_H
#define __MIDIINSTANCE_H
class MidiInstance
{
  static MidiInstance instance;
public:
  enum RPN
  {
    PitchBendSensitivity,
    ChannelFineTuning,
    ChannelCoarseTuning,
    SelectTuningProgram,
    SelectTuningBank,
    ModulationDepthRange
  };
  enum NRPN
  {
    NRPN_VibratoRate=136,
    NRPN_VibratoDepth=137,
    NRPN_VibratoDelay=138,
    NRPN_FilterCutoffFreq=160,
    NRPN_FilterResonance=161,
    NRPN_EGAttackTime=227,
    NRPN_EGDecayTime=228,
    NRPN_EGReleaseTime=230
  };
  midi::MidiInterface<HardwareSerial, midi::DefaultSettings>* MIDI;
  static MidiInstance* it() { return &instance; }
  MidiInstance();
};
#endif
