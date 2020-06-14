#include "Config.h"
#include "Display.h"
#include "Control.h"
#include "AssignableController.h"
#include "EditMode.h"

#include "MidiInstance.h"

typedef enum {
  enBankR1_4,
  enBankR5_8
} Bank;
Bank bank;

#define MAX_RVAL_INDEX 8
#define RVAL_BANK_SIZE 4

/*
 * MIDI messages
 */
// Sends master volume in the range 0-127 (MSB only)

/*
 * Keyboard MUX handler
 */
#define KB_NUMROWS 8
#define KB_NUMCOLS 8
const byte kbRows[]={53,49,45,41,51,47,43,39};
const byte kbCols[]={52,50,48,46,44,42,40,38};
#define MIN_BTN2_INDEX 32

byte kbSwitchStates[KB_NUMROWS*KB_NUMCOLS];
byte old_kbSwitchStates[KB_NUMROWS*KB_NUMCOLS];

const char* note_names[]={
  "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
  "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
  "C3"
};

void setupKeys()
{
  for (int i=0; i<KB_NUMCOLS; i++)
  {
    pinMode(kbCols[i],OUTPUT);
    digitalWrite(kbCols[i],LOW);
  } 
  for (int i=0; i<KB_NUMROWS; i++)
    pinMode(kbRows[i],INPUT);
  memset(old_kbSwitchStates,LOW,KB_NUMROWS*KB_NUMCOLS);
}

unsigned long millisContact[MIN_BTN2_INDEX];

void loopKeys()
{
  digitalWrite(kbCols[KB_NUMCOLS-1],LOW);
  for (int c=0; c<KB_NUMCOLS; c++)
  {
    if (c==0)
      digitalWrite(kbCols[KB_NUMCOLS-1],LOW);
    else
      digitalWrite(kbCols[c-1],LOW);
    digitalWrite(kbCols[c],HIGH);
    //delay(1);
    for (int r=0; r<KB_NUMROWS; r++)
      kbSwitchStates[r*KB_NUMCOLS+c]=digitalRead(kbRows[r]);
  }
  for (int i=0; i<KB_NUMROWS*KB_NUMCOLS; i++)
  {
#if SERIAL_DEBUG
    if (kbSwitchStates[i]==HIGH)
    {
      Serial.print(note_names[i]);
      Serial.print(" ");
    }
#endif
    if (kbSwitchStates[i]!=old_kbSwitchStates[i])
    {
      if (i<MIN_BTN2_INDEX)
      {

        if (EditMode::it()->isActive())
          EditMode::it()->queueKeyAction(i,kbSwitchStates[i]==HIGH);
        else
        {
          if (kbSwitchStates[i]==HIGH)
            millisContact[i]=millis();
          else
            MidiInstance::it()->MIDI->sendNoteOn(Controllers::it()->getMiddleC()-12+i,0,Controllers::it()->getChannel());
        }
      }
      else
      {
        if (kbSwitchStates[i]==HIGH)
        {
          byte velocity=(32-min(millis()-millisContact[i-MIN_BTN2_INDEX],28))*4;
          MidiInstance::it()->MIDI->sendNoteOn(Controllers::it()->getMiddleC()-12+(i-MIN_BTN2_INDEX),velocity,Controllers::it()->getChannel());
        }
      }
      old_kbSwitchStates[i]=kbSwitchStates[i];
    }
  }
}
/*
 * Button MUX handler
 */
#define BTN_NUMROWS 6
#define BTN_NUMCOLS 2
const int btnRows[]={2,7,3,4,5,6};
const int btnCols[]={A9,A11};
byte btnSwitchStates[BTN_NUMROWS*BTN_NUMCOLS];
byte old_btnSwitchStates[BTN_NUMROWS*BTN_NUMCOLS];
// Button names numbered by r*BTN_NUMCOLS+c
// r=0 c=0 => K3 0
#define BTN_DATA_MINUS 0
// r=0 c=1 => K2 1
#define BTN_DATA_PLUS 1
// r=1 c=0 => K4 2
#define BTN_CTRL_SWITCH 2
// r=1 c=1 => K1 3
#define BTN_EDIT 3
// r=2 c=0 => K11L
#define BTN_R15L 4
// r=2 c=1 => K11R
#define BTN_R15R 5
// r=3 c=0 => K12L
#define BTN_R26L 6
// etc.
#define BTN_R26R 7
#define BTN_R37L 8
#define BTN_R37R 9
#define BTN_R48L 10
#define BTN_R48R 11
#define BTN_MINR  BTN_R15L
#define BTN_MAXR  BTN_R48R

void setupButtons()
{
  bank=enBankR1_4;
  for (int i=0; i<BTN_NUMCOLS; i++)
  {
    pinMode(btnCols[i],OUTPUT);
    digitalWrite(btnCols[i],LOW);
  } 
  for (int i=0; i<BTN_NUMROWS; i++)
    pinMode(btnRows[i],INPUT);
  memset(old_btnSwitchStates,LOW,BTN_NUMROWS*BTN_NUMCOLS);
}

void loopButtons()
{
  digitalWrite(btnCols[BTN_NUMCOLS-1],LOW);
  for (int c=0; c<BTN_NUMCOLS; c++)
  {
    if (c==0)
      digitalWrite(btnCols[BTN_NUMCOLS-1],LOW);
    else
      digitalWrite(btnCols[c-1],LOW);
    digitalWrite(btnCols[c],HIGH);
    //delay(1);
    for (int r=0; r<BTN_NUMROWS; r++)
    {
      btnSwitchStates[r*BTN_NUMCOLS+c]=digitalRead(btnRows[r]);
    }
  }
  for (int i=0; i<BTN_NUMROWS*BTN_NUMCOLS; i++)
  {
    if (btnSwitchStates[i]!=old_btnSwitchStates[i])
    {
      // Rotary encoders
      if (i>=BTN_MINR && i<=BTN_MAXR)
      {
        int index=(i-BTN_MINR)/2;
        bool increment=true;
        char value;
        if (bank==enBankR5_8)
          index+=RVAL_BANK_SIZE;
        // The rotary encoder moves through the following states for the two switches:
        // AB = 00 01 11 10
        // So, if A changes to a state where A==B, or B changes to a state where A!=B
        // then that is one direction.
        // A changing to A!=B, or B changing to A==B is the other direction.
        if ((i-BTN_MINR)%2==0)  // A
        {
          if (btnSwitchStates[i]==btnSwitchStates[i+1])
            increment=false;
        }
        else  // B
        {
          if (btnSwitchStates[i-1]!=btnSwitchStates[i])
            increment=false;
        }
        if (increment)
          value=AssignableControllers::it()->getController(AssignableControllers::R1+index)->increment();
        else
          value=AssignableControllers::it()->getController(AssignableControllers::R1+index)->decrement();
      }
      // Push-buttons
      else if (btnSwitchStates[i]==HIGH)
      {
        switch (i)
        {
          case BTN_DATA_MINUS:
#if SERIAL_DEBUG
            Serial.print("DATA-");
#endif    
            AssignableControllers::it()->getController(AssignableControllers::DATA)->decrement();
            break;
          case BTN_DATA_PLUS:
#if SERIAL_DEBUG          
            Serial.print("DATA+");
#endif        
            AssignableControllers::it()->getController(AssignableControllers::DATA)->increment();
            break;
          case BTN_EDIT:
            EditMode::it()->toggle();
            if (!EditMode::it()->isActive())
              Controllers::it()->getController(Controllers::CONTROL_OCTAVE)->displayValue(0);
#if SERIAL_DEBUG          
            Serial.print("EDIT");
#endif            
            break;
          case BTN_CTRL_SWITCH:
            if (bank==enBankR1_4)
              bank=enBankR5_8;
            else
              bank=enBankR1_4;
#if SERIAL_DEBUG          
            Serial.print("SWITCH");
#endif            
            break;
        }
      }
      old_btnSwitchStates[i]=btnSwitchStates[i];
    }
  }
}

/*
 * Analog handler
 */
#define ANA_WHEEL2 0
#define ANA_WHEEL1 1
#define ANA_SLIDER 2
#define ANA_SIZE 3
#define WHEEL1_DEADZONE 32
#define WHEEL1_MID 512
#define W1DZ_MIN (WHEEL1_MID-WHEEL1_DEADZONE)
#define W1DZ_MAX (WHEEL1_MID+WHEEL1_DEADZONE)
#define W2_TOP_DEADZONE 8
#define W2_MAX (1024-W2_TOP_DEADZONE)

int lastAnaValue[ANA_SIZE];
const int anaPort[ANA_SIZE]={A0,A1,A2};

void setupAnalog()
{
  for (int i=0; i<ANA_SIZE; i++)
    lastAnaValue[i]=-1;
}

void loopAnalog()
{
  for (int i=0; i<ANA_SIZE; i++)
  {
    int value=analogRead(anaPort[i]);
    if (i==ANA_WHEEL1 && value>=W1DZ_MIN && value<=W1DZ_MAX)
      value=WHEEL1_MID;
    if (i==ANA_WHEEL2 && value>W2_MAX)
      value=W2_MAX;
    if (value!=lastAnaValue[i])
    {
      lastAnaValue[i]=value;
      if (i==ANA_SLIDER)
        value=AssignableControllers::it()->getController(AssignableControllers::SLIDER)->setValue(value,0,1023);
      if (i==ANA_WHEEL2)
        value=AssignableControllers::it()->getController(AssignableControllers::WHEEL2)->setValue(value,0,W2_MAX);
      if (i==ANA_WHEEL1)
        value=AssignableControllers::it()->getController(AssignableControllers::WHEEL1)->setValue(value,0,1023);
    }
  }
}

#if SERIAL_DEBUG
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

#define MEM_DEBUG(tag) Serial.print(tag);Serial.println(freeMemory());Serial.flush();
#else
#define MEM_DEBUG(tag)
#endif

/*
 * Main program
 */
void setup() {
#if SERIAL_DEBUG
  Serial.begin(115200);
#endif
  MEM_DEBUG("/0:");
  setupKeys();
  MEM_DEBUG("/1:");
  setupButtons();
  MEM_DEBUG("/2:");
  Display::it()->setup();
  MEM_DEBUG("/3:");
  Display::it()->setupTest();
  MEM_DEBUG("/4:");
  setupAnalog();
  bank=enBankR1_4;
  EditMode::it()->setup();
  MEM_DEBUG("/5:");
  Controllers::it()->setup();
  MEM_DEBUG("/6:");
  AssignableControllers::it()->setup();
  MEM_DEBUG("/7:");
  MidiInstance::it()->MIDI->begin();
  MEM_DEBUG("/8:");
#if SERIAL_DEBUG==0
  Serial.begin(115200);
#endif
 }

void loop() {
  loopKeys();
  loopButtons();
#if !LED_TEST
  // Update any status LEDs that should have changed state
  Display::it()->setLED(Display::LED_R1R4,(bank==enBankR1_4)?Display::LED_ON:Display::LED_OFF);
  Display::it()->setLED(Display::LED_R5R8,(bank==enBankR5_8)?Display::LED_ON:Display::LED_OFF);
  Display::it()->setLED(Display::LED_EDIT,EditMode::it()->isActive()?Display::LED_ON:Display::LED_OFF);
  Display::it()->setLED(Display::LED_DATA_PLUS,(!EditMode::it()->isActive() && Controllers::it()->getMiddleC()>DEFAULT_MIDDLE_C)?Display::LED_ON:Display::LED_OFF);
  Display::it()->setLED(Display::LED_DATA_MINUS,(!EditMode::it()->isActive() && Controllers::it()->getMiddleC()<DEFAULT_MIDDLE_C)?Display::LED_ON:Display::LED_OFF);
#endif
  Display::it()->loop();
  Display::it()->loopTest();
  loopAnalog();
  EditMode::it()->loop();
#if SERIAL_DEBUG  
  Serial.println();
#endif  
}
