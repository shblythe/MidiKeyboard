#include <Arduino.h>
#include "Control.h"

class AssignableController
{
private:
  byte mNumber;
  byte mChannel;
  byte mMinParameter;
  byte mMaxParameter;
  byte mDefaultControl;
  byte mDefaultChannel;
  Control* mControl;
  byte getChannel() { return mChannel!=0 ? mChannel : Controllers::it()->getChannel(); }
public:
  AssignableController(byte number, byte defaultChannel, byte minParameter, byte maxParameter, byte defaultControl)
  {
    mNumber=number;
    mChannel=mDefaultChannel=defaultChannel;
    mMinParameter=minParameter;
    mMaxParameter=maxParameter;
    mDefaultControl=defaultControl;
    setDefaultControl();
  }

  bool setDefaultControl()
  {
    mControl=Controllers::it()->getController(mDefaultControl);
  }

  bool setControl(byte index)
  {
    if (index<mMinParameter || index>mMaxParameter)
      return false;
    mControl=Controllers::it()->getController(index);
    return true;
  }
  void setChannel(byte channel) { mChannel=channel; }
  char setValue(char value) { return mControl->setValue(getChannel(),value); }
  char setValue(int srcValue, int srcMin, int srcMax) { return mControl->setValue(getChannel(),srcValue,srcMin,srcMax);}
  char increment() { return mControl->setValue(getChannel(),mControl->getValue()+1); }
  char decrement() { return mControl->setValue(getChannel(),mControl->getValue()-1); }
};

class AssignableControllers
{
public:
  enum { DATA, SLIDER, WHEEL1, WHEEL2, PEDALA, PEDALB, R1, R2, R3, R4, R5, R6, R7, R8, NUMACS };
private:
  AssignableController* mControllers[NUMACS];
  static AssignableControllers instance;
public:
  static AssignableControllers* it() { return &instance; }
  AssignableController* getController(byte index) { return mControllers[index]; }
  void setup();
};
