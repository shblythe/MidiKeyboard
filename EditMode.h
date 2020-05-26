#include <Arduino.h>
#include "AssignableController.h"

#ifndef __EDITMODE_H
#define __EDITMODE_H
class EditMode
{
  static EditMode instance;
  enum KeyFunctions
  {
	  KEY_ASSIGN,
	  KEY_CHANNEL,
	  KEY_PROGRAM,
	  KEY_BANK_MSB,
	  KEY_BANK_LSB,
	  KEY_DUAL,
	  KEY_MTC,
	  KEY_MUTE,
	  KEY_SNAPSHOT,
	  KEY_UPLOAD,
	  KEY_DOWNLOAD,
	  KEY_ALL_NOTE_OFF,
	  KEY_RESET_ALL_CONTROLLER,
	  KEY_GMGSXG_ON,
	  KEY_0,
	  KEY_1,
	  KEY_2,
	  KEY_3,
	  KEY_4,
	  KEY_5,
	  KEY_6,
	  KEY_7,
	  KEY_8,
	  KEY_9,
	  KEY_ENTER,
	  KEY_NONE
  };
  char keyPressed;
  AssignableController *assignableController;
  bool active;
	unsigned long errorStartMs;
	const unsigned long errorLengthMs=1000;
  enum States
  {
	  STATE_EDITMENU,
	  STATE_ASSIGN,
	  STATE_ASSIGN_CHOOSE=STATE_ASSIGN,
	  STATE_ASSIGN_CHOSEN,
	  STATE_ASSIGN_NUM_ENTRY,
		STATE_ERROR,
		STATE_ERROR_DELAY,
  } state;
	char numEntry[Display::LED_NUMDIGITS];
	byte numDigitsEntered;

public:
  static EditMode* it() { return &instance; }
  EditMode() { }
  void queueKeyAction(int keyNumber, bool pressed) { if (pressed) keyPressed=keyNumber; }
  void toggle() { active=!active; keyPressed=KEY_NONE; state=STATE_EDITMENU; }
  bool isActive() { return active; }
  void setup() { active=false; keyPressed=KEY_NONE; }
  void loop();
  void assignController(AssignableController* a);
};
#endif
