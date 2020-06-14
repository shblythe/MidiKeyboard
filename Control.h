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
  char mLastValue[MAX_CHANNEL];
  virtual char setControlValue(byte channel, char value)=0;

public:
  Control(byte number, char defaultValue, char maximum=127, char minimum=0)
  {
    mNumber=number;
    mMinimum=minimum;
    mMaximum=maximum;
    mDefaultValue=defaultValue;
    for (int i=0; i<MAX_CHANNEL; i++)
      mLastValue[i]=defaultValue;
  }

  // This version of setValue chops at min/max, therefore assuming the caller already
  // knows the min/max range, or is happy to be corrected.
  virtual byte setValue(byte channel, char value)
  {
    if (value==-128 || value>mMaximum)
      value=mMaximum;
    else if (value<mMinimum)
      value=mMinimum;
    if (value!=mLastValue[channel])
    {
      value=setControlValue(channel,value);
      if (value!=mLastValue[channel])
      {
        mLastValue[channel]=value;
        displayValue(channel);
      }
    }
    return value;
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

  virtual char getValue(byte channel) { return mLastValue[channel]; }
  virtual void displayValue(byte channel) {};
  byte getNumber() { return mNumber; }
};

class DisplayedControl : public Control
{
private:
  byte mNumDigits;
public:
  DisplayedControl(byte number, char defaultValue, char maximum=127, char minimum=0, byte numDigits=3)
    :Control(number, defaultValue, maximum, minimum),mNumDigits(numDigits) {}
  virtual void displayValue(byte channel)
  {
    Display::it()->displayLEDsValue(mLastValue[channel],mNumDigits);
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

class MidiRPNControl:public DisplayedControl
{
  int mRpnNumber;

  using DisplayedControl::DisplayedControl;
protected:
  virtual char setControlValue(byte channel, char value)
  {
    MidiInstance::it()->MIDI->beginRpn(mRpnNumber,channel);
    MidiInstance::it()->MIDI->sendRpnValue(value,channel);
    MidiInstance::it()->MIDI->endRpn(channel);
    return value;
  }

public:
  MidiRPNControl(int number, int rpnNumber, char defaultValue, char maximum=127, char minimum=0, byte numDigits=3)
    :DisplayedControl(number,defaultValue,maximum,minimum,numDigits)
  {
    mRpnNumber=rpnNumber;
  }
};

class MidiNRPNControl:public DisplayedControl
{
  int mNrpnNumber;

protected:
  virtual char setControlValue(byte channel, char value)
  {
    MidiInstance::it()->MIDI->beginNrpn(mNrpnNumber,channel);
    MidiInstance::it()->MIDI->sendNrpnValue(value,channel);
    MidiInstance::it()->MIDI->endNrpn(channel);
  }

public:
  MidiNRPNControl(int number, int nrpnNumber, char defaultValue, char maximum=127, char minimum=0, byte numDigits=3)
    :DisplayedControl(number,defaultValue,maximum,minimum,numDigits)
  {
    mNrpnNumber=nrpnNumber;
  }
};

// Just stores a value, doesn't send anything anywhere
class ValueControl : public DisplayedControl
{
  using DisplayedControl::DisplayedControl;
protected:
  virtual char setControlValue(byte channel, char value) {return value;}
public:
  // Just use channel 0 for all operations, since these values don't relate to channels
  virtual byte setValue(byte channel, char value)
  {
    return DisplayedControl::setValue(0,value);
  }
  virtual char getValue(byte channel) { return DisplayedControl::getValue(0); }
};

class Controllers
{
private:
  Control* mControls[160];
  static Controllers instance;
  static const char Controllers::MidiControlDefaultValues[];
public:
  enum {  CONTROL_RPN_PITCH_BEND_SENSITIVITY=128,
          CONTROL_RPN_CHANNEL_FINE_TUNING,
          CONTROL_RPN_CHANNEL_COARSE_TUNING,
          CONTROL_RPN_MODULATION_DEPTH_RANGE,
          CONTROL_NRPN_VIBRATO_RATE,
          CONTROL_NRPN_VIBRATO_DEPTH,
          CONTROL_NRPN_VIBRATO_DELAY,
          CONTROL_NRPN_FILTER_CUTOFF_FREQUENCY,
          CONTROL_NRPN_FILTER_RESONANCE,
          CONTROL_NRPN_EQ_LOW_GAIN,
          CONTROL_NRPN_EQ_HIGH_GAIN,
          CONTROL_NRPN_EQ_LOW_FREQUENCY,
          CONTROL_NRPN_EQ_HIGH_FREQUENCY,
          CONTROL_NRPN_EG_ATTACK_TIME,
          CONTROL_NRPN_EG_DECAY_TIME,
          CONTROL_NRPN_EG_RELEASE_TIME,
          CONTROL_POLYPHONIC_KEY_PRESSURE,
          CONTROL_AFTER_TOUCH,
          CONTROL_PITCH_BEND,
          CONTROL_MASTER_VOLUME,
          CONTROL_START_MTC,
          CONTROL_CONTINUE_MTC,
          CONTROL_STOP_MTC,
          CONTROL_RESET_MTC,
          CONTROL_PROGRAM,
          CONTROL_CHANNEL,
          CONTROL_OCTAVE,
          CONTROL_TRANSPOSE,
          CONTROL_TEMPO,
          CONTROL_KEYBOARD_CURVE,
          CONTROL_PEDAL_A_CURVE,
          CONTROL_PEDAL_B_CURVE };

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
    return mControls[CONTROL_OCTAVE]->getValue(0)*12+mControls[CONTROL_TRANSPOSE]->getValue(0)+DEFAULT_MIDDLE_C;
  }

  char getKeyboardCurve()
  {
    return mControls[CONTROL_KEYBOARD_CURVE]->getValue(0);
  }
};
