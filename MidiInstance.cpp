#include <Arduino.h>
#include "MidiInstance.h"
#include "Config.h"

static MidiInstance MidiInstance::instance;

MidiInstance::MidiInstance()
{
#if MIDI_HAIRLESS
  MIDI=new midi::MidiInterface<HardwareSerial,midi::DefaultSettings>(Serial);
#else
  MIDI=new midi::MidiInterface<HardwareSerial,midi::DefaultSettings>(Serial1);
#endif
}
