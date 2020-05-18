#include <MIDI.h>

// These two options are mutually exclusive, since they both use the same serial port!
#define SERIAL_DEBUG 0
#define MIDI_HAIRLESS 1
#if (SERIAL_DEBUG && MIDI_HAIRLESS)
#error
#endif 

#define LED_TEST 0

#if MIDI_HAIRLESS
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial,Serial,MIDI,midi::DefaultSettings);
#else
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial,Serial1,MIDI,midi::DefaultSettings);
#endif
#define DEFAULT_MIDDLE_C 60
int middleC;

typedef enum {
  enBankR1_4,
  enBankR5_8
} Bank;
Bank bank;

bool editMode;

#define MAX_RVAL_INDEX 8
byte rVal[MAX_RVAL_INDEX];
#define MAX_RVAL 127
#define RVAL_BANK_SIZE 4

/*
 * MIDI messages
 */
// Sends master volume in the range 0-127 (MSB only)
int channelNum;

void sendMasterVolume(byte vol)
{
  byte msg[6]={0x7f, 0x7f, 0x04, 0x01, vol, 0};
  MIDI.sendSysEx(6,msg,false);
}

void sendPitchBend(byte bend)
{
  int b=bend;
  b-=64;
  b*=128;
  MIDI.sendPitchBend(b,channelNum);
}

void sendModulationWheel(byte mod)
{
  MIDI.sendControlChange(midi::ModulationWheel,mod,channelNum);
}

/*
 * Keyboard MUX handler
 */
#define KB_NUMROWS 4
#define KB_NUMCOLS 8
const byte kbRows[]={53,49,45,41,51,47,43,39};
const byte kbCols[]={52,50,48,46,44,42,40,38};

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

void loopKeys()
{
  if (editMode)
    return;
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
      MIDI.sendNoteOn(middleC-12+i,(kbSwitchStates[i]==HIGH)?64:0,channelNum);
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
typedef struct Control
{
  byte channel;
  byte controlNumber;
};
Control rControls[]={
  { 1, 152 },
  { 1, 153 },
  { 1, 156 },
  { 1, 157 },
  { 1,   7 },
  { 2,   7 },
  { 3,   7 },
  { 4,   7 }
};
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

#define MAX_MIDI_CONTROL 127
byte updateControl(byte number, byte value, byte channel)
{
  byte rval=value;
  if (number>MAX_MIDI_CONTROL)
  {
    switch (number)
    {
      case 152: // PROGRAM : Not yet implemented
        break;
      case 153: // CHANNEL
        if (value>16)
          value=16;
        if (value<1)
          value=1;
        channelNum=value;
        rval=value;
        break;
      case 156: // TEMPO : Not yet implemented
        break;
      case 157: // KEYBOARD CURVE : Not yet implemented
        break;
    }
  }
  else
    MIDI.sendControlChange(number,value,channel);
  return rval;
}

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
        {
          if (rVal[index]<MAX_RVAL)
            rVal[index]++;
        }
        else if (rVal[index]>0)
          rVal[index]--;
        rVal[index]=updateControl(rControls[index].controlNumber,rVal[index],rControls[index].channel);
        displayLEDsValue(rVal[index]);
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
            // Don't let it get so low that bottom C
            // is out of midi range
            if (middleC>=24)
              middleC-=12;
            break;
          case BTN_DATA_PLUS:
#if SERIAL_DEBUG          
            Serial.print("DATA+");
#endif        
            // Don't let it get so high that the top C
            // is out of midi range    
            if (middleC<=103)
              middleC+=12;
            break;
          case BTN_EDIT:
            editMode=!editMode;
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
 * LED MUX handler
 */
#define LED_NUMROWS 4   // anodes
#define LED_NUMCOLS 8   // cathodes
#define LEDROWON HIGH
#define LEDROWOFF LOW
#define LEDCOLON LOW
#define LEDCOLOFF HIGH
const int ledRows[] = { A3, 21, 16, 14 };
const int ledCols[] = { 17, 15, A7, A5, A4, 20, A10, A6 };
const byte digitSegs[10][7] = {
  //        a  b  c  d  e  f  g
  /* 0 */ { 1, 1, 1, 1, 1, 1, 0 },
  /* 1 */ { 0, 1, 1, 0, 0, 0, 0 },
  /* 2 */ { 1, 1, 0, 1, 1, 0, 1 },
  /* 3 */ { 1, 1, 1, 1, 0, 0, 1 },
  /* 4 */ { 0, 1, 1, 0, 0, 1, 1 },
  /* 5 */ { 1, 0, 1, 1, 0, 1, 1 },
  /* 6 */ { 1, 0, 1, 1, 1, 1, 1 },
  /* 7 */ { 1, 1, 1, 0, 0, 0, 0 },
  /* 8 */ { 1, 1, 1, 1, 1, 1, 1 },
  /* 9 */ { 1, 1, 1, 1, 0, 1, 1 }
};
byte ledStates[LED_NUMROWS*LED_NUMCOLS];
int ledRow;
// Individual LEDs defined by index of r*LED_NUMCOLS+c
#define LED_R5R8 4
#define LED_EDIT 0
#define LED_R1R4 3
#define LED_DATA_MINUS 2
#define LED_DATA_PLUS 1
#define LED_DIG1_DP 15
#define LED_DIG2_DP 23
#define LED_DIG3_DP 31

void setDigit(int index, int value)
{
  memcpy(&ledStates[(index+1)*LED_NUMCOLS],digitSegs[value],7);
}

void displayLEDsValue(int value)
{
  setDigit(2,value%10);
  value/=10;
  setDigit(1,value%10);
  value/=10;
  setDigit(0,value);
}

void setupLEDs() {
  for (int i=0; i<LED_NUMROWS; i++)
  {
    pinMode(ledRows[i],OUTPUT);
    digitalWrite(ledRows[i],LEDROWOFF);
  }
  for (int i=0; i<LED_NUMCOLS; i++)
  {
    pinMode(ledCols[i],OUTPUT);
    digitalWrite(ledCols[i],LEDCOLOFF);
  }
  memset(ledStates,LOW,LED_NUMROWS*LED_NUMCOLS);
  ledRow=0;
}

void loopLEDs() {
  // Update any status LEDs that should have changed state
  ledStates[LED_R1R4]=(bank==enBankR1_4)?1:0;
  ledStates[LED_R5R8]=(bank==enBankR5_8)?1:0;
  ledStates[LED_EDIT]=editMode?1:0;
  ledStates[LED_DATA_PLUS]=(!editMode && middleC>DEFAULT_MIDDLE_C)?1:0;
  ledStates[LED_DATA_MINUS]=(!editMode && middleC<DEFAULT_MIDDLE_C)?1:0;

  // Just one row each time this is called, to leave them on for a bit
  digitalWrite(ledRows[ledRow>0?(ledRow-1):(LED_NUMROWS-1)],LEDROWOFF);
  for (int c=0; c<LED_NUMCOLS; c++)
    digitalWrite(ledCols[c],ledStates[ledRow*LED_NUMCOLS+c]?LEDCOLON:LEDCOLOFF);
  digitalWrite(ledRows[ledRow],LEDROWON);
  ledRow++;
  if (ledRow>=LED_NUMROWS)
    ledRow=0;
}

#if LED_TEST
const byte testLEDs[]={LED_EDIT, LED_DATA_PLUS, LED_DATA_MINUS, LED_DIG1_DP, LED_DIG2_DP, LED_DIG3_DP, LED_R1R4, LED_R5R8 };
#define NUM_TEST_LEDS 8
unsigned long nextTestLEDsStep;
int testLEDsIndex;
int testDigits=0;
#define TEST_LED_STEP_MS 500
void setupTestCycleLEDs() {
  nextTestLEDsStep=0;
  testLEDsIndex=0;
}
void loopTestCycleLEDs() {
  if (millis()>nextTestLEDsStep)
  {
    ledStates[testLEDs[testLEDsIndex>0?(testLEDsIndex-1):(NUM_TEST_LEDS-1)]]=0;
    ledStates[testLEDs[testLEDsIndex]]=1;
    nextTestLEDsStep+=TEST_LED_STEP_MS;
    testLEDsIndex++;
    if (testLEDsIndex>=NUM_TEST_LEDS)
      testLEDsIndex=0;
    displayLEDsValue(testDigits);
    testDigits++;
    if (testDigits>999)
      testDigits=0;
  }
}
#else
inline void setupTestCycleLEDs() {}
inline void loopTestCycleLEDs() {}
#endif

/*
 * Analog handler
 */
#define ANA_MODULATION 0
#define ANA_PITCHBEND 1
#define ANA_MASTERVOL 2
#define ANA_SIZE 3
#define PITCHBEND_DEADZONE 4
#define PITCHBEND_MID 64
#define PBDZ_MIN (PITCHBEND_MID-PITCHBEND_DEADZONE)
#define PBDZ_MAX (PITCHBEND_MID+PITCHBEND_DEADZONE)
#define MODULATION_TOP_DEADZONE 1
#define MOD_MAX (127-MODULATION_TOP_DEADZONE)

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
    int value=analogRead(anaPort[i])>>3;
    if (i==ANA_PITCHBEND && value>=PBDZ_MIN && value<=PBDZ_MAX)
      value=PITCHBEND_MID;
    if (i==ANA_MODULATION && value>MOD_MAX)
      value=MOD_MAX;
    if (value!=lastAnaValue[i])
    {
      if (i!=ANA_PITCHBEND)
        displayLEDsValue(value);
      lastAnaValue[i]=value;
      if (i==ANA_MASTERVOL)
        sendMasterVolume(value);
      if (i==ANA_PITCHBEND)
        sendPitchBend(value);
      if (i==ANA_MODULATION)
        sendModulationWheel(value);
    }
  }
}
/*
 * Main program
 */
void setup() {
  setupKeys();
  setupButtons();
  setupLEDs();
  setupTestCycleLEDs();
  setupAnalog();
  middleC=DEFAULT_MIDDLE_C;
  bank=enBankR1_4;
  editMode=false;
  channelNum=1;
  for (int i=0; i<MAX_RVAL_INDEX; i++)
    rVal[i]=0;
  MIDI.begin();
  Serial.begin(115200);
 }

void loop() {
  loopKeys();
  loopButtons();
  loopLEDs();
  loopTestCycleLEDs();
  loopAnalog();
#if SERIAL_DEBUG  
  Serial.println();
#endif  
}
