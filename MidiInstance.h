#include <Arduino.h>
#include <MIDI.h>

#ifndef __MIDIINSTANCE_H
#define __MIDIINSTANCE_H
class MidiInstance
{
  static MidiInstance instance;
public:
  midi::MidiInterface<HardwareSerial, midi::DefaultSettings>* MIDI;
  static MidiInstance* it() { return &instance; }
  MidiInstance();
};
#endif
