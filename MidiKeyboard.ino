#include <MIDI.h>

// These two options are mutually exclusive, since they both use the same serial port!
#define SERIAL_DEBUG 0
#define MIDI_HAIRLESS 1

#define KB_NUMROWS 4
#define KB_NUMCOLS 8
const byte kbRows[]={53,49,45,41,51,47,43,39};
const byte kbCols[]={52,50,48,46,44,42,40,38};
int middleC;

byte kbSwitchStates[KB_NUMROWS*KB_NUMCOLS];
byte old_kbSwitchStates[KB_NUMROWS*KB_NUMCOLS];

const char* note_names[]={
  "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
  "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
  "C3"
};

#define BTN_NUMROWS 2
#define BTN_NUMCOLS 2
const int btnRows[]={A8,7};
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

#if MIDI_HAIRLESS
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial,Serial,MIDI,midi::DefaultSettings);
#else
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial,Serial1,MIDI,midi::DefaultSettings);
#endif

void setup() {
  // put your setup code here, to run once:
  for (int i=0; i<KB_NUMCOLS; i++)
  {
    pinMode(kbCols[i],OUTPUT);
    digitalWrite(kbCols[i],LOW);
  } 
  for (int i=0; i<KB_NUMROWS; i++)
    pinMode(kbRows[i],INPUT);
  memset(old_kbSwitchStates,LOW,KB_NUMROWS*KB_NUMCOLS);
  for (int i=0; i<BTN_NUMCOLS; i++)
  {
    pinMode(btnCols[i],OUTPUT);
    digitalWrite(btnCols[i],LOW);
  } 
  for (int i=0; i<BTN_NUMROWS; i++)
    pinMode(btnRows[i],INPUT);
  memset(old_btnSwitchStates,LOW,BTN_NUMROWS*BTN_NUMCOLS);
  middleC=0x30;
  MIDI.begin();
  Serial.begin(115200);
}

void loop() {
  // Keyboard
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
      MIDI.sendNoteOn(middleC+i,(kbSwitchStates[i]==HIGH)?64:0,1);
      old_kbSwitchStates[i]=kbSwitchStates[i];
    }
  }
  // Buttons
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
      if (btnSwitchStates[i]==HIGH)
      {
        switch (i)
        {
          case BTN_DATA_MINUS:
#if SERIAL_DEBUG
            Serial.print("DATA-");
#endif    
            middleC-=12;
            break;
          case BTN_DATA_PLUS:
#if SERIAL_DEBUG          
            Serial.print("DATA+");
#endif            
            middleC+=12;
            break;
          case BTN_EDIT:
#if SERIAL_DEBUG          
            Serial.print("EDIT");
#endif            
            break;
          case BTN_CTRL_SWITCH:
#if SERIAL_DEBUG          
            Serial.print("SWITCH");
#endif            
            break;
        }
      }
      old_btnSwitchStates[i]=btnSwitchStates[i];
    }
  }
#if SERIAL_DEBUG  
  Serial.println();
#endif  
}
