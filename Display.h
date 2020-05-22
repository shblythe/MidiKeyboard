#include "Arduino.h"
#include "Config.h"
#ifndef __DISPLAY_H
#define __DISPLAY_H

class Display
{
private:
  static const byte LED_NUMROWS=4;   // anodes
  static const byte LED_NUMCOLS=8;   // cathodes
  static const byte LEDROWON=HIGH;
  static const byte LEDROWOFF=LOW;
  static const byte LEDCOLON=LOW;
  static const byte LEDCOLOFF=HIGH;
  static const int ledRows[];
  static const int ledCols[];
  static const byte digitSegs[][7];
  byte ledStates[LED_NUMROWS*LED_NUMCOLS];
  int ledRow;
#if LED_TEST
  static const byte testLEDs[];
#endif

  static Display instance;
  void setDigit(int index, int value);
  
public:
  // Individual LEDs defined by index of r*LED_NUMCOLS+c
  // Should be enums?
  static const byte LED_R5R8=4;
  static const byte LED_EDIT=0;
  static const byte LED_R1R4=3;
  static const byte LED_DATA_MINUS=2;
  static const byte LED_DATA_PLUS=1;
  static const byte LED_DIG1_DP=15;
  static const byte LED_DIG2_DP=23;
  static const byte LED_DIG3_DP=31;
  static const byte LED_ON=1;
  static const byte LED_OFF=0;

  static Display* it() { return &instance; }
  void displayLEDsValue(int value, int digits=3);
  void setLED(byte address, byte state);
  void setup();
  void loop();
  void setupTest();
  void loopTest();
};

#endif
