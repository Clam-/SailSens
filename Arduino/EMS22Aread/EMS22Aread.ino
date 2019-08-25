// Encoder 1
const int ENC1_CS = 4; // Blue
const int ENC1_DATA = 3; //Green
const int ENC1_CLOCK = 2; //Yellow
// Encoder 2
const int ENC2_CS = 7;
const int ENC2_DATA = 6;
const int ENC2_CLOCK = 5;
// Encoder 3
const int ENC3_CS = 10;
const int ENC3_DATA = 9;
const int ENC3_CLOCK = 8;

const float STEP = 0.35294;
unsigned int OPS = 0;
unsigned int PREVOPS = 0;
unsigned long PREVTIME = 0;
unsigned long OPSTIME = 1000;

void initEnc(int csPin, int clkPin, int dPin) {
  pinMode(csPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  pinMode(dPin, INPUT);
  digitalWrite(clkPin, HIGH);
  digitalWrite(csPin, LOW);
}

void setup() {
  SerialUSB.begin(115200);
  initEnc(ENC1_CS, ENC1_CLOCK, ENC1_DATA); // Init Encoder 1
  initEnc(ENC2_CS, ENC2_CLOCK, ENC2_DATA); // Init Encoder 2
  initEnc(ENC3_CS, ENC3_CLOCK, ENC3_DATA); // Init Encoder 3
  Serial3.begin(9600);
}

int readEncoder(int csPin, int clkPin, int dPin) {
  digitalWrite(csPin, HIGH);
  digitalWrite(csPin, LOW);
  int pos = 0;
  for (int i=0; i<16; i++) { 
    digitalWrite(clkPin, LOW);
    digitalWrite(clkPin, HIGH);
   
    pos = pos | digitalRead(dPin);
    if (i<15) { pos = pos << 1; }
  }
  digitalWrite(clkPin, LOW);
  digitalWrite(clkPin, HIGH);
  return pos; // including extdata
}

unsigned int parity_check(unsigned int v){
  //http://graphics.stanford.edu/~seander/bithacks.html#ParityNaive
  v ^= v >> 16;
  v ^= v >> 8;
  v ^= v >> 4;
  v &= 0xf;
  return (0x6996 >> v) & 1;
}

//byte stream[16];
void loop() {
  unsigned int enc1 = readEncoder(ENC1_CS, ENC1_CLOCK, ENC1_DATA);
  unsigned int enc1v = enc1 >> 6;
  unsigned int enc1e = enc1 & B111111;
  SerialUSB.print(enc1, HEX);
  SerialUSB.print("\t"); 
  SerialUSB.print(enc1v, DEC); 
  SerialUSB.print("\t"); 
  SerialUSB.print(enc1e, HEX); 
  SerialUSB.print("\t"); 
  SerialUSB.println(parity_check(enc1), BIN);

  unsigned int enc2 = readEncoder(ENC2_CS, ENC2_CLOCK, ENC2_DATA);
  unsigned int enc3 = readEncoder(ENC3_CS, ENC3_CLOCK, ENC3_DATA);
  // Debug format: enc1  enc2  enc3  ram1  ram2  ops
  Serial3.print(enc1); Serial3.print(" "); Serial3.print(enc2); Serial3.print(" "); Serial3.print(enc3); Serial3.print(" "); 
  Serial3.print(0); Serial3.print(" "); Serial3.print(0); Serial3.print(" "); Serial3.println(OPS);

  //ops calc
  if (millis() - PREVTIME > OPSTIME){
    PREVOPS = OPS; OPS = 0;
  } else { OPS++; }
  PREVTIME = millis();
}
