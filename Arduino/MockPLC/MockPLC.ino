#include <Wire.h>

#include <Modbus.h>
#include <ModbusSerial.h>
// https://github.com/epsilonrt/modbus-arduino

#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 DAC1;
Adafruit_MCP4725 DAC2;

// Modbus config
#define SLAVE_ID   1
const int SPIN_STATUS = 1; // 12129
const int RAM1_REG    = 20; // 40641
const int RAM2_REG    = 30; // 40642
const int MAINSH_REG  = 40; // 400643
const int TILL_REG    = 50; // 40644
const int HEEL_REG    = 60; // 40645
ModbusSerial mb;

// Encoder 1
const int ENC1_CS = 4;      // Blue
const int ENC1_DATA = 3;    // Orange
const int ENC1_CLOCK = 2;   // Yellow
const int ENC1_LEDZ = 14;     // A0 LED Zeroing
const int ENC1_LEDA = 15;     // A1 LED Activity (mod 2)
// Encoder 2
const int ENC2_CS = 5;     // Blue
const int ENC2_DATA = 6;    // Orange
const int ENC2_CLOCK = 7;   // Yellow
const int ENC2_LEDZ = 16;     // A2 LED Zeroing
const int ENC2_LEDA = 17;     // A3 LED Activity (mod 2)
// Encoder 3
const int ENC3_CS = 8;     // Blue
const int ENC3_DATA = 9;   // Orange
const int ENC3_CLOCK = 10;  // Yellow
const int ENC3_LEDZ = 18;     // A4 LED Zeroing
const int ENC3_LEDA = 19;     // A5 LED Activity (mod 2)

const int SPIN = 13;        // Spinnaker

const float STEP = 0.35294;

int RAM[] = {0,4095};
int RAMinc[] = {0, -1};

void initEnc(int csPin, int clkPin, int dPin) {
  pinMode(csPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  pinMode(dPin, INPUT);
  digitalWrite(clkPin, HIGH);
  digitalWrite(csPin, LOW);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Connected "); 

  //Serial1.begin(38400);
  mb.config(&Serial1, 9600, MB_PARITY_NONE);
  mb.setSlaveId(SLAVE_ID);
  mb.addIsts(SPIN_STATUS, false);
  mb.addHreg(RAM1_REG);
  mb.addHreg(RAM2_REG);
  mb.addHreg(MAINSH_REG);
  mb.addHreg(TILL_REG);
  mb.addHreg(HEEL_REG);
  
   
  // For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
  // For MCP4725A0 the address is 0x60 or 0x61
  // For MCP4725A2 the address is 0x64 or 0x65
  DAC1.begin(0x62);
  DAC2.begin(0x63);
  
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

void incRAM(int index) {
  if (RAM[index] > 4094) { RAMinc[index] = -1; }
  if (RAM[index] < 1) { RAMinc[index] = +1; }
  RAM[index] = RAM[index] + RAMinc[index];
}

void processLED(int enc, int LEDA, int LEDZ){
  if ((enc %5) == 0)
    digitalWrite(LEDA, HIGH);
  else
    digitalWrite(LEDA, LOW);
  if (enc < 600 && enc > 500)
    digitalWrite(LEDZ, HIGH);
  else
    digitalWrite(LEDZ, LOW);
}

//byte stream[16];
void loop() {
  mb.task();
  
  int enc1 = readEncoder(ENC1_CS, ENC1_CLOCK, ENC1_DATA);
  int enc2 = readEncoder(ENC2_CS, ENC2_CLOCK, ENC2_DATA);
  int enc3 = readEncoder(ENC3_CS, ENC3_CLOCK, ENC3_DATA);

  DAC1.setVoltage(mb.Hreg(RAM1_REG), false);
  DAC2.setVoltage(mb.Hreg(RAM1_REG), false);
  incRAM(0);
  incRAM(1);

  // LED update
  processLED(enc1, ENC1_LEDA, ENC1_LEDZ);
  processLED(enc2, ENC2_LEDA, ENC2_LEDZ);
  processLED(enc3, ENC3_LEDA, ENC3_LEDZ);
    
  // set modbus data
  mb.Ists(SPIN_STATUS, digitalRead(SPIN));
  mb.Hreg(MAINSH_REG, enc1);
  mb.Hreg(TILL_REG, enc2);
  mb.Hreg(HEEL_REG, enc3);
}
