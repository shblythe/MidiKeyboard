#include <Arduino.h>
#include "MidiInstance.h"

#define MAX_MIDI_CONTROL 127

class Control
{
protected:
  byte mNumber;  // Controller number
  byte mMinimum; // minimum value
  byte mMaximum; // maximum value
  byte mDefaultValue;
  byte mValue;
  virtual byte setControlValue(byte channel, byte value)=0;

public:
  Control(byte number, byte defaultValue, byte maximum=127, byte minimum=0)
  {
    mNumber=number;
    mMinimum=minimum;
    mMaximum=maximum;
    mDefaultValue=defaultValue;
    mValue=defaultValue;
  }

  byte setValue(byte channel, byte value)
  {
    if (value<mMinimum)
      value=mMinimum;
    else if (value>mMaximum)
      value=mMaximum;
    mValue=setControlValue(channel,value);
    return mValue;
  }

  byte sendDefault()
  {
    
  }
};

class MidiControl : public Control
{
protected:
  virtual byte setControlValue(byte channel, byte value)
  {
    MidiInstance::it()->MIDI->sendControlChange(mNumber,value,channel);
    return value;
  }

public:
  MidiControl(byte number, byte defaultValue, byte maximum=127, byte minimum=0)
        :Control(number,defaultValue,maximum,minimum) {};
};

class Controllers
{
private:
  Control* mControls[159];
  static Controllers instance;
  static const byte Controllers::MidiControlDefaultValues[];
public:
  static Controllers* it() { return &instance; }
  void setup();
  void loop();
};
