#include "Display.h"

const int Display::ledRows[4] = { A3, 21, 16, 14 };
const int Display::ledCols[8] = { 17, 15, A7, A5, A4, 20, A10, A6 };
const byte Display::digitSegs[12][7] = {
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
  /* 9 */ { 1, 1, 1, 1, 0, 1, 1 },
  /* 10*/ { 0, 0, 0, 0, 0, 0, 1 }, // for minus sign
  /* 11*/ { 0, 0, 0, 0, 0, 0, 0 }  // for space
};
const byte Display::msgSegs[][LED_NUMDIGITS][7]={
  //  MSG_CHOOSE  - CHO
  {
    { 1, 0, 0, 1, 1, 1, 0 },  // C
    { 0, 1, 1, 0, 1, 1, 1 },  // H
    { 1, 1, 1, 1, 1, 1, 0 },  // O
  },
  //  MSG_ON      - _On
  {
    { 0, 0, 0, 0, 0, 0, 0 },  // space
    { 1, 1, 1, 1, 1, 1, 0 },  // O
    { 1, 1, 1, 0, 1, 1, 0 },  // n
  },
  //  MSG_OFF     - OFF
  {
    { 1, 1, 1, 1, 1, 1, 0 },  // O
    { 1, 0, 0, 0, 1, 1, 1 },  // F
    { 1, 0, 0, 0, 1, 1, 1 },  // F
  },
  //  MSG_DONE    - dOn
  {
    { 0, 1, 1, 1, 1, 0, 1 },  // d
    { 1, 1, 1, 1, 1, 1, 0 },  // O
    { 1, 1, 1, 0, 1, 1, 0 },  // n
  },
  //  MSG_ERROR   - Err
  {
    { 1, 0, 0, 1, 1, 1, 1 },  // E
    { 0, 0, 0, 0, 1, 0, 1 },  // r
    { 0, 0, 0, 0, 1, 0, 1 },  // r
  },
  //  MSG_SETTING_UPLOAD    - SEu
  {
    { 1, 0, 1, 1, 0, 1, 1 },  // S
    { 1, 0, 0, 1, 1, 1, 1 },  // E
    { 0, 0, 1, 1, 1, 0, 0 },  // u
  },
  //  MSG_SETTING_DOWNLOAD  - SEd
  {
    { 1, 0, 1, 1, 0, 1, 1 },  // S
    { 1, 0, 0, 1, 1, 1, 1 },  // E
    { 0, 1, 1, 1, 1, 0, 1 },  // d
  },
  //  MSG_BLANK
  {
    { 0, 0, 0, 0, 0, 0, 0 },  // space
    { 0, 0, 0, 0, 0, 0, 0 },  // space
    { 0, 0, 0, 0, 0, 0, 0 },  // space
  }
};
#define DIGIT_MINUS 10
#define DIGIT_SPACE 11
Display Display::instance;

void Display::setSegs(int index, const byte* segList)
{
  memcpy(&ledStates[(index+1)*LED_NUMCOLS],segList,7);
}

void Display::setDigit(int index, int value)
{
  setSegs(index,digitSegs[value]);
}

void Display::displayMessage(int index)
{
  for (int i=0; i<LED_NUMDIGITS; i++)
    setSegs(i,msgSegs[index][i]);
}

void Display::displayNumString(char* digits, byte numDigits)
{
  for (int i=0; i<numDigits && i<LED_NUMDIGITS; i++)
    setDigit(i,digits[i]);
  if (numDigits<LED_NUMDIGITS)
    for (int i=numDigits; i<LED_NUMDIGITS; i++)
      setDigit(i,DIGIT_SPACE);
}

void Display::displayLEDsValue(int value, int digits=3)
{
  bool negative=value<0;
  value=abs(value);
  setDigit(2,value%10);
  value/=10;
  setDigit(1,digits>1?value%10:DIGIT_SPACE);
  value/=10;
  setDigit(0,negative?DIGIT_MINUS:(digits>2?value:DIGIT_SPACE));
}

void Display::setLED(byte address, byte state)
{
  ledStates[address]=state;
}

void Display::setup() {
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

void Display::loop() {

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
static const byte Display::testLEDs[]={LED_EDIT, LED_DATA_PLUS, LED_DATA_MINUS, LED_DIG1_DP, LED_DIG2_DP, LED_DIG3_DP, LED_R1R4, LED_R5R8 };
#define NUM_TEST_LEDS 8
unsigned long nextTestLEDsStep;
int testLEDsIndex;
int testDigits=0;
#define TEST_LED_STEP_MS 500
void Display::setupTest() {
  nextTestLEDsStep=0;
  testLEDsIndex=0;
}
void Display::loopTest() {
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
void Display::setupTest() {}
void Display::loopTest() {}
#endif
