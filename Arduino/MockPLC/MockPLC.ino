//#define DEBUG
//#define SERIALOUT
#define MONITOR

#include <Wire.h>

#include <Modbus.h>
#include <ModbusSerial.h>
// https://github.com/epsilonrt/modbus-arduino

#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 exDAC1;
Adafruit_MCP4725 exDAC2;

unsigned long t1 = 0;
unsigned long t2 = 0;
int ops = 0;

// Modbus config
#define SLAVE_ID   1
// Offsets are actually -1 from CAS Modbus Scanner
const int SPIN_STATUS = 2128; // 12129
const int RAM1_REG    = 641; // 40641
const int RAM2_REG    = 640; // 40642
const int MAINSH_REG  = 642; // 400643
const int TILL_REG    = 643; // 40644
const int HEEL_REG    = 644; // 40645
#ifndef DEBUG
ModbusSerial mb;
#endif

// Encoder 1  MainSheet
const int ENC1_CS = 4;      // Blue
const int ENC1_DATA = 3;    // Orange
const int ENC1_CLOCK = 2;   // Yellow
const int ENC1_LEDZ = A0;     // A0 LED Zeroing
const int ENC1_LEDA = A1;     // A1 LED Activity
// Encoder 2  Rudder
const int ENC2_CS = 7;     // Blue
const int ENC2_DATA = 6;    // Orange
const int ENC2_CLOCK = 5;   // Yellow
const int ENC2_LEDZ = A2;     // A2 LED Zeroing
const int ENC2_LEDA = A3;     // A3 LED Activity
// Encoder 3  Heel
const int ENC3_CS = 10;     // Blue
const int ENC3_DATA = 9;   // Orange
const int ENC3_CLOCK = 8;  // Yellow
const int ENC3_LEDZ = A4;     // A4 LED Zeroing
const int ENC3_LEDA = A5;     // A5 LED Activity

const int SPIN = 11;        // Spinnaker

const float STEP = 0.35294;

//int RAM[] = {0,4095};
//int RAMinc[] = {0, -1};

void initEnc(int csPin, int clkPin, int dPin) {
  pinMode(csPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  pinMode(dPin, INPUT);
  digitalWrite(clkPin, HIGH);
  digitalWrite(csPin, LOW);
}

void setup() {
#ifdef SERIALOUT
  Serial1.begin(9600);
  Serial1.println("Connected "); 
#endif
#ifdef MONITOR
  Serial2.begin(38400);
#endif

#ifndef DEBUG
  mb.config(&SerialUSB, 38400); //SERIAL_8N1
  mb.setSlaveId(SLAVE_ID);
  mb.addIsts(SPIN_STATUS, false);
  mb.addHreg(RAM1_REG);
  mb.addHreg(RAM2_REG);
  mb.addIreg(MAINSH_REG);
  mb.addIreg(TILL_REG);
  mb.addIreg(HEEL_REG);
  mb.addIreg(0);
  mb.addIreg(1);  
#endif
   
  // For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
  // For MCP4725A0 the address is 0x60 or 0x61
  // For MCP4725A2 the address is 0x64 or 0x65
  exDAC1.begin(0x62);
  exDAC2.begin(0x63);

  exDAC1.setVoltage(0, true);
  exDAC2.setVoltage(0, true);
  
  initEnc(ENC1_CS, ENC1_CLOCK, ENC1_DATA); // Init Encoder 1
  initEnc(ENC2_CS, ENC2_CLOCK, ENC2_DATA); // Init Encoder 2
  initEnc(ENC3_CS, ENC3_CLOCK, ENC3_DATA); // Init Encoder 3

  // Setup LED pins
  pinMode(ENC1_LEDZ, OUTPUT); digitalWrite(ENC1_LEDZ, LOW);
  pinMode(ENC1_LEDA, OUTPUT); digitalWrite(ENC1_LEDA, LOW);
  pinMode(ENC2_LEDZ, OUTPUT); digitalWrite(ENC2_LEDZ, LOW);
  pinMode(ENC2_LEDA, OUTPUT); digitalWrite(ENC2_LEDA, LOW);
  pinMode(ENC3_LEDZ, OUTPUT); digitalWrite(ENC3_LEDZ, LOW);
  pinMode(ENC3_LEDA, OUTPUT); digitalWrite(ENC3_LEDA, LOW);
  pinMode(SPIN, INPUT);
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

void processLED(int enc, int LEDA, int LEDZ){
  if ((enc & B00001100) > 0)
    digitalWrite(LEDA, HIGH);
  else
    digitalWrite(LEDA, LOW);
  if (enc < 600 && enc > 500)
    digitalWrite(LEDZ, HIGH);
  else
    digitalWrite(LEDZ, LOW);
}

void printOps() {
  t1 = millis();
  if (t1 > t2) {
    t2 = t1 + 1000;
    Serial.print("Ops: "); Serial.println(ops);
    ops = 0;
  }
  ops += 1;
}

void loop() {
#ifndef DEBUG
  mb.task();
#endif
  
  int enc1 = readEncoder(ENC1_CS, ENC1_CLOCK, ENC1_DATA);
  int enc2 = readEncoder(ENC2_CS, ENC2_CLOCK, ENC2_DATA);
  int enc3 = readEncoder(ENC3_CS, ENC3_CLOCK, ENC3_DATA);

//  Serial.print("Enc1: "); Serial.print((enc1& B00000110) > 0); Serial.print("Enc2: "); 
//  Serial.print((enc2 & B00001110) > 0); Serial.print("Enc3: "); Serial.println((enc3 & B00001100) > 0);

#ifdef SERIALOUT
  Serial1.print("RAM1:"); Serial1.print(mb.Hreg(RAM1_REG)); Serial1.print("  RAM2:"); Serial1.println(mb.Hreg(RAM2_REG)); 
#endif
#ifdef MONITOR
  Serial2.print(enc1); Serial2.print(" "); Serial2.print(enc2); Serial2.print(" "); Serial2.print(enc3); Serial2.print(" "); 
  Serial2.print(mb.Hreg(RAM1_REG)); Serial2.print(" "); Serial2.println(mb.Hreg(RAM2_REG)); 
#endif

#ifndef DEBUG
  //normal code  
  exDAC1.setVoltage(mb.Hreg(RAM1_REG), false);
  exDAC2.setVoltage(mb.Hreg(RAM2_REG), false);
#else
  // debug DAC code

#endif

  // LED update
  processLED(enc1, ENC1_LEDA, ENC1_LEDZ);
  processLED(enc2, ENC2_LEDA, ENC2_LEDZ);
  processLED(enc3, ENC3_LEDA, ENC3_LEDZ);

#ifdef DEBUG
  Serial.print("Spin: ");Serial.println(digitalRead(SPIN));  
#else
  // set modbus data
  mb.Ists(SPIN_STATUS, digitalRead(SPIN));
  mb.Ireg(MAINSH_REG, enc1);
  mb.Ireg(TILL_REG, enc2);
  mb.Ireg(HEEL_REG, enc3);
#endif
  //printOps
}
