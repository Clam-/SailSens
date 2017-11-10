// Encoder 1
const int ENC1_CS = 2;
const int ENC1_CLOCK = 3;
const int ENC1_DATA = 5;
// Encoder 2
const int ENC1_CS = 6;
const int ENC1_CLOCK = 9;
const int ENC1_DATA = 10;
// Encoder 3
const int ENC1_CS = 11;
const int ENC1_CLOCK = 12;
const int ENC1_DATA = 13;

const float STEP = 0.35294;

void initEnc(int csPin, int clkPin, int dPin) {
  pinMode(csPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  pinMode(dPin, INPUT);
  digitalWrite(clkPin, HIGH);
  digitalWrite(csPin, LOW);
}

void setup() {
  Serial.begin(115200);
  initEnc(ENC1_CS, ENC1_CLOCK, ENC1_DATA); // Init Encoder 1
  initEnc(ENC2_CS, ENC2_CLOCK, ENC2_DATA); // Init Encoder 2
  initEnc(ENC3_CS, ENC3_CLOCK, ENC3_DATA); // Init Encoder 3
}

int readEncoder(int csPin, int clkPin, int dPin) {
  digitalWrite(csPin, HIGH);
  digitalWrite(csPin, LOW);
  int pos = 0;
  for (int i=0; i<10; i++) {
    digitalWrite(clkPin, LOW);
    digitalWrite(clkPin, HIGH);
   
    byte b = digitalRead(dPin) == HIGH ? 1 : 0;
    pos += b * pow(2, 10-(i+1));
  }
  for (int i=0; i<6; i++) {
    digitalWrite(clkPin, LOW);
    digitalWrite(clkPin, HIGH);
  }
  digitalWrite(clkPin, LOW);
  digitalWrite(clkPin, HIGH);
  return pos;
}

//byte stream[16];
void loop() {
  Serial.print(readEncoder(ENC1_CS, ENC1_CLOCK, ENC1_DATA)*STEP); 
  Serial.print("\t"); 
  Serial.print(readEncoder(ENC2_CS, ENC2_CLOCK, ENC2_DATA)*STEP); 
  Serial.print("\t"); 
  Serial.println(readEncoder(ENC3_CS, ENC3_CLOCK, ENC3_DATA)*STEP);
}
