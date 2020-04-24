#define NUMROWS 8
#define NUMCOLS 8
const byte rows[]={53,49,45,41,51,47,43,39};
const byte cols[]={52,50,48,46,44,42,40,38};

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
      if (digitalRead(rows[r]))
      {
        Serial.print(r);
        Serial.print(" ");
        Serial.println(c);
      }
  }
}
