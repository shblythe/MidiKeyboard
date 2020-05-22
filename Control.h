#include <Arduino.h>
#include "MidiInstance.h"
#include "Config.h"
#include "Display.h"

#define MAX_MIDI_CONTROL 127
#define MAX_CHANNEL 16

#pragma once

class Control
{
protected:
  byte mNumber;  // Controller number
  char mMinimum; // minimum value
  char mMaximum; // maximum value
  char mDefaultValue;
  char mValue[MAX_CHANNEL];
  char mLastValue[MAX_CHANNEL];
  virtual char setControlValue(byte channel, char value)=0;
  virtual void displayValue(byte channel) {};

public:
  Control(byte number, char defaultValue, char maximum=127, char minimum=0)
  {
    mNumber=number;
    mMinimum=minimum;
    mMaximum=maximum;
    mDefaultValue=defaultValue;
    for (int i=0; i<MAX_CHANNEL; i++)
      mLastValue[i]=mValue[i]=defaultValue;
  }

  // This version of setValue chops at min/max, therefore assuming the caller already
  // knows the min/max range, or is happy to be corrected.
  byte setValue(byte channel, char value)
  {
    if (value==-128 || value>mMaximum)
      value=mMaximum;
    else if (value<mMinimum)
      value=mMinimum;
    mValue[channel]=setControlValue(channel,value);
    if (mValue[channel]!=mLastValue[channel])
    {
      displayValue(channel);
      mLastValue[channel]=mValue[channel];
    }
    return mValue[channel];
  }
  // This version allows the caller to pass the range of the source of the value, e.g.
  // it could be an analog input in the range 0-1023.  From that, we will spread the range
  // of our value, e.g. so that with a value range of 0-127, it will be 127 when the passed
  // input is 1023
  byte setValue(byte channel, int srcValue, int srcMin, int srcMax)
  {
    long value=(long)srcValue*(mMaximum-mMinimum+1);
    value/=(srcMax-srcMin+1);
    return setValue(channel,value);
  }

  char getValue(byte channel) { return mValue[channel]; }
};

class DisplayedControl : public Control
{
private:
  byte mNumDigits;
public:
  DisplayedControl(byte number, char defaultValue, char maximum=127, char minimum=0, byte numDigits=3)
    :Control(number, defaultValue, maximum, minimum),mNumDigits(numDigits) {}
protected:
  virtual void displayValue(byte channel)
  {
    Display::it()->displayLEDsValue(mValue[channel],mNumDigits);
  }
};

class MidiControl : public DisplayedControl
{
  using DisplayedControl::DisplayedControl;
protected:
  virtual char setControlValue(byte channel, char value)
  {
    MidiInstance::it()->MIDI->sendControlChange(mNumber,value,channel);
    return value;
  }
};

class MasterVolumeControl : public DisplayedControl
{
  using DisplayedControl::DisplayedControl;
protected:
  virtual char setControlValue(byte channel, char value)
  {
    byte msg[6]={0x7f, 0x7f, 0x04, 0x01, value, 0};
    MidiInstance::it()->MIDI->sendSysEx(6,msg,false);
    return value;
  }
};

class PitchBendControl:public Control
{
  using Control::Control;
protected:
  virtual char setControlValue(byte channel, char value)
  {
    int b=value;
    b-=64;
    b*=128;
    MidiInstance::it()->MIDI->sendPitchBend(b,channel);
    return value;
  }
};

class ProgramControl:public DisplayedControl
{
  using DisplayedControl::DisplayedControl;
protected:
  virtual char setControlValue(byte channel, char value)
  {
    MidiInstance::it()->MIDI->sendProgramChange(value,channel);  
    return value;
  }
};

// Just stores a value, doesn't send anything anywhere
class ValueControl : public DisplayedControl
{
  using DisplayedControl::DisplayedControl;
protected:
  virtual char setControlValue(byte channel, char value) {return value;}
};

class Controllers
{
private:
  Control* mControls[159];
  static Controllers instance;
  static const char Controllers::MidiControlDefaultValues[];
public:
  enum { CONTROL_PITCH_BEND=146, CONTROL_MASTER_VOLUME, CONTROL_PROGRAM=152, CONTROL_CHANNEL, CONTROL_OCTAVE, CONTROL_TEMPO=156, CONTROL_KEYBOARD_CURVE=157 };

  static Controllers* it() { return &instance; }
  void setup();
  Control* getController(int index)
  {
    return mControls[index];
  }
  
  char getControlValue(int channel, int index)
  {
    return mControls[index]->getValue(channel);
  }

  // Convenience functions
  char getChannel()
  {
    return mControls[CONTROL_CHANNEL]->getValue(0);
  }

  char getMiddleC()
  {
    return mControls[CONTROL_OCTAVE]->getValue(0)*12+DEFAULT_MIDDLE_C;
  }
};
