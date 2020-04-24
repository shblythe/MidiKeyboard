#define NUMROWS 4
#define NUMCOLS 8
const byte rows[]={53,49,45,41,51,47,43,39};
const byte cols[]={52,50,48,46,44,42,40,38};

const char* note_names[]={
  "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
  "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
  "C3"
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  for (int i=0; i<NUMCOLS; i++)
  {
    pinMode(cols[i],OUTPUT);
    digitalWrite(cols[i],LOW);
  } 
  for (int i=0; i<NUMROWS; i++)
    pinMode(rows[i],INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(cols[NUMCOLS-1],LOW);
  for (int c=0; c<NUMCOLS; c++)
  {
    if (c==0)
      digitalWrite(cols[NUMCOLS-1],LOW);
    else
      digitalWrite(cols[c-1],LOW);
    digitalWrite(cols[c],HIGH);
    //delay(1);
    for (int r=0; r<NUMROWS; r++)
    {
      if (digitalRead(rows[r]))
      {
        Serial.print(note_names[r*NUMCOLS+c]);
        Serial.print(" ");
      }
    }
  }
  Serial.println();
}
