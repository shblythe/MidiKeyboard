#include <Arduino.h>
#include "Control.h"

#pragma once
class EditMode;

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

  byte getControlNumber()
  {
    return mControl->getNumber();
  }

  bool setControl(byte index)
  {
    if (index<mMinParameter || index>mMaxParameter)
      return false;
    mControl=Controllers::it()->getController(index);
    return true;
  }
  void setChannel(byte channel) { mChannel=channel; }
  char setValue(char value) {
    if (!checkEditMode())
      return mControl->setValue(mChannel,value);
    return 0;
  }
  char setValue(int srcValue, int srcMin, int srcMax) { 
    if (!checkEditMode())
      return mControl->setValue(mChannel,srcValue,srcMin,srcMax);
    return 0;
  }

  char increment()
  {
    if (!checkEditMode())
      return mControl->setValue(mChannel,mControl->getValue(mChannel)+1);
    return 0;
  }

  char decrement()
  {
    if (!checkEditMode())
      return mControl->setValue(mChannel,mControl->getValue(mChannel)-1);
    return 0;
  }

  bool checkEditMode();
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
