// #################################
// COMPILER OPTIONS, debug etc.
// #################################
//#define DEBUG
//#define SERIALOUT
#define MONITOR
//#define FIRSTTIMESETUP

// #################################
// INCLUDES, REQUIREMENTS
// #################################
#include <Wire.h>

#include <Modbus.h>
#include <ModbusSerial.h> 
// https://github.com/Clam-/modbus-arduino
// Modified from https://github.com/epsilonrt/modbus-arduino

#include <Adafruit_MCP4725.h>

#include "GPIO.h"
#include "Button.h"

// #################################
// DEFINITIONS
// #################################

Adafruit_MCP4725 exDAC1;
Adafruit_MCP4725 exDAC2;

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

// #################################
// PIN CONFIGURATION
// #################################
Button<BOARD::D12> spin_button; // Spinnaker
bool spinnaker = false;

// Encoder 1  MainSheet
GPIO<BOARD::D4>     ENC1_CS;
GPIO<BOARD::D3>     ENC1_DATA;
GPIO<BOARD::D2>     ENC1_CLOCK;
GPIO<BOARD::D40>     ENC1_LED1;
GPIO<BOARD::D38>     ENC1_LED2;
GPIO<BOARD::D36>     ENC1_LED3;

// Encoder 2  Rudder
GPIO<BOARD::D7>     ENC2_CS;
GPIO<BOARD::D6>     ENC2_DATA;
GPIO<BOARD::D5>     ENC2_CLOCK;
GPIO<BOARD::D34>     ENC2_LED1;
GPIO<BOARD::D32>     ENC2_LED2;
GPIO<BOARD::D30>     ENC2_LED3;

// Encoder 3  Heel
GPIO<BOARD::D10>    ENC3_CS;
GPIO<BOARD::D9>     ENC3_DATA;
GPIO<BOARD::D8>     ENC3_CLOCK;
GPIO<BOARD::D28>     ENC3_LED1;
GPIO<BOARD::D26>     ENC3_LED2;
GPIO<BOARD::D24>     ENC3_LED3;

GPIO<BOARD::D13>     TEST_LED;

int FI = 0;
bool FLASH() {
  if (FI > 10) {return true;}
  return false;
}
void FLASH_ITER() {
  if (FI > 20) { FI = 0; }
  else { FI++; }
}

int VALUES[] = {0,1024,0,1024, 0};
int VALUESinc[] = {1, -1, 1, -1, 1};

int v1 = 0;
int v2 = 0;

void initEnc() {
  // Set mode of encoder pins
  ENC1_CS.output(); ENC2_CS.output(); ENC3_CS.output();
  ENC1_CLOCK.output(); ENC2_CLOCK.output(); ENC3_CLOCK.output();
  ENC1_DATA.input(); ENC2_DATA.input(); ENC3_DATA.input(); 
  
  // Set Encoder initial states
  ENC1_CLOCK = HIGH; ENC2_CLOCK = HIGH; ENC3_CLOCK = HIGH; 
  ENC1_CS = LOW;     ENC2_CS = LOW;     ENC3_CS = LOW;

  // Set LED pin modes
  ENC1_LED1.output(); ENC1_LED2.output(); ENC1_LED3.output();
  ENC2_LED1.output(); ENC2_LED2.output(); ENC2_LED3.output();
  ENC3_LED1.output(); ENC3_LED2.output(); ENC3_LED3.output();
  ENC1_LED1 = LOW; ENC1_LED2 = LOW; ENC1_LED3 = LOW;
  ENC2_LED1 = LOW; ENC2_LED2 = LOW; ENC2_LED3 = LOW;
  ENC3_LED1 = LOW; ENC3_LED2 = LOW; ENC3_LED3 = LOW;
}

int MONSTEP = 0;

void setup() {
#ifdef SERIALOUT
  Serial1.begin(9600);
  Serial1.println("Connected "); 
#endif
#ifdef MONITOR
  Serial3.begin(9600);
  //Serial3.println("Opened Serial3."); 
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
#ifdef FIRSTTIMESETUP
  exDAC1.setVoltage(0, true);
  exDAC2.setVoltage(0, true);
#else
  exDAC1.setVoltage(0, false);
  exDAC2.setVoltage(0, false);
#endif
  initEnc(); // Init Encoders
  TEST_LED.output();
}

int readENC1() {
  // ENC1_CS ENC1_DATA ENC1_CLOCK;
  ENC1_CS = HIGH; ENC1_CS = LOW;
  int pos = 0;
  for (int i=0; i<10; i++) {
    ENC1_CLOCK = LOW; ENC1_CLOCK = HIGH;
    byte b = ENC1_DATA.read() == HIGH ? 1 : 0;
    pos += b * pow(2, 10-(i+1));
  }
  for (int i=0; i<6; i++) { ENC1_CLOCK = LOW; ENC1_CLOCK = HIGH; } // skip these bits
  ENC1_CLOCK = LOW; ENC1_CLOCK = HIGH;
  return pos;
}
int readENC2() {
  // ENC2_CS ENC2_DATA ENC2_CLOCK;
  ENC2_CS = HIGH; ENC2_CS = LOW;
  int pos = 0;
  for (int i=0; i<10; i++) {
    ENC2_CLOCK = LOW; ENC2_CLOCK = HIGH;
    byte b = ENC2_DATA.read() == HIGH ? 1 : 0;
    pos += b * pow(2, 10-(i+1));
  }
  for (int i=0; i<6; i++) { ENC2_CLOCK = LOW; ENC2_CLOCK = HIGH; } // skip these bits
  ENC2_CLOCK = LOW; ENC2_CLOCK = HIGH;
  return pos;
}
int readENC3() {
  // ENC3_CS ENC3_DATA ENC3_CLOCK;
  ENC3_CS = HIGH; ENC3_CS = LOW;
  int pos = 0;
  for (int i=0; i<10; i++) {
    ENC3_CLOCK = LOW; ENC3_CLOCK = HIGH;
    byte b = ENC3_DATA.read() == HIGH ? 1 : 0;
    pos += b * pow(2, 10-(i+1));
  }
  for (int i=0; i<6; i++) { ENC3_CLOCK = LOW; ENC3_CLOCK = HIGH; } // skip these bits
  ENC3_CLOCK = LOW; ENC3_CLOCK = HIGH;
  return pos;
}

void doENC1LEDs(int v){
  // ENC1_LED1 ENC1_LED2 ENC1_LED3
  if (v < 85) { ENC1_LED1 = LOW; ENC1_LED2 = LOW; ENC1_LED3 = LOW; }
  else if (v >= 85 && v < 170) { 
    if (FLASH()) { ENC1_LED1 = HIGH; } else { ENC1_LED1 = LOW; }
    ENC1_LED2 = LOW; 
    ENC1_LED3 = LOW; 
  } else if (v >= 170 && v < 255) { ENC1_LED1 = HIGH; ENC1_LED2 = LOW; ENC1_LED3 = LOW; }
  else if (v >= 255 && v < 340) { 
    ENC1_LED1 = HIGH; 
    if (FLASH()) { ENC1_LED2 = HIGH; } else { ENC1_LED2 = LOW; }
    ENC1_LED3 = LOW; 
  } else if (v >= 340 && v < 425) { ENC1_LED1 = HIGH; ENC1_LED2 = HIGH; ENC1_LED3 = LOW; }
  else if (v >= 425 && v < 510) { 
    if (FLASH()) { ENC1_LED1 = HIGH; } else { ENC1_LED1 = LOW; }
    ENC1_LED2 = HIGH;
    ENC1_LED3 = LOW;
  } else if (v >= 510 && v < 595) { ENC1_LED1 = LOW; ENC1_LED2 = HIGH; ENC1_LED3 = LOW; }
  else if (v >= 595 && v < 680) { 
    ENC1_LED1 = LOW;
    ENC1_LED2 = HIGH;
    if (FLASH()) { ENC1_LED3 = HIGH; } else { ENC1_LED3 = LOW; }
  } else if (v >= 680 && v < 765) { ENC1_LED1 = LOW; ENC1_LED2 = HIGH; ENC1_LED3 = HIGH; }
  else if (v >= 765 && v < 850) { 
    ENC1_LED1 = LOW;
    if (FLASH()) { ENC1_LED2 = HIGH; } else { ENC1_LED2 = LOW; }
    ENC1_LED3 = HIGH;
  } else if (v >= 850 && v < 935) { ENC1_LED1 = LOW; ENC1_LED2 = LOW; ENC1_LED3 = HIGH; }
  else if (v >= 935) { 
    ENC1_LED1 = LOW;
    ENC1_LED2 = LOW;
    if (FLASH()) { ENC1_LED3 = HIGH; } else { ENC1_LED3 = LOW; }
  }
}
void doENC2LEDs(int v){
  // ENC2_LED1 ENC2_LED2 ENC2_LED3
  if (v < 85) { ENC2_LED1 = LOW; ENC2_LED2 = LOW; ENC2_LED3 = LOW; }
  else if (v >= 85 && v < 170) { 
    if (FLASH()) { ENC2_LED1 = HIGH; } else { ENC2_LED1 = LOW; }
    ENC2_LED2 = LOW; 
    ENC2_LED3 = LOW; 
  } else if (v >= 170 && v < 255) { ENC2_LED1 = HIGH; ENC2_LED2 = LOW; ENC2_LED3 = LOW; }
  else if (v >= 255 && v < 340) { 
    ENC2_LED1 = HIGH; 
    if (FLASH()) { ENC2_LED2 = HIGH; } else { ENC2_LED2 = LOW; }
    ENC2_LED3 = LOW; 
  } else if (v >= 340 && v < 425) { ENC2_LED1 = HIGH; ENC2_LED2 = HIGH; ENC2_LED3 = LOW; }
  else if (v >= 425 && v < 510) { 
    if (FLASH()) { ENC2_LED1 = HIGH; } else { ENC2_LED1 = LOW; }
    ENC2_LED2 = HIGH;
    ENC2_LED3 = LOW;
  } else if (v >= 510 && v < 595) { ENC2_LED1 = LOW; ENC2_LED2 = HIGH; ENC2_LED3 = LOW; }
  else if (v >= 595 && v < 680) { 
    ENC2_LED1 = LOW;
    ENC2_LED2 = HIGH;
    if (FLASH()) { ENC2_LED3 = HIGH; } else { ENC2_LED3 = LOW; }
  } else if (v >= 680 && v < 765) { ENC2_LED1 = LOW; ENC2_LED2 = HIGH; ENC2_LED3 = HIGH; }
  else if (v >= 765 && v < 850) { 
    ENC2_LED1 = LOW;
    if (FLASH()) { ENC2_LED2 = HIGH; } else { ENC2_LED2 = LOW; }
    ENC2_LED3 = HIGH;
  } else if (v >= 850 && v < 935) { ENC2_LED1 = LOW; ENC2_LED2 = LOW; ENC2_LED3 = HIGH; }
  else if (v >= 935) { 
    ENC2_LED1 = LOW;
    ENC2_LED2 = LOW;
    if (FLASH()) { ENC2_LED3 = HIGH; } else { ENC2_LED3 = LOW; }
  }
}
void doENC3LEDs(int v){
  // ENC3_LED1 ENC3_LED2 ENC3_LED3
  if (v < 85) { ENC3_LED1 = LOW; ENC3_LED2 = LOW; ENC3_LED3 = LOW; }
  else if (v >= 85 && v < 170) { 
    if (FLASH()) { ENC3_LED1 = HIGH; } else { ENC3_LED1 = LOW; }
    ENC3_LED2 = LOW; 
    ENC3_LED3 = LOW; 
  } else if (v >= 170 && v < 255) { ENC3_LED1 = HIGH; ENC3_LED2 = LOW; ENC3_LED3 = LOW; }
  else if (v >= 255 && v < 340) { 
    ENC3_LED1 = HIGH; 
    if (FLASH()) { ENC3_LED2 = HIGH; } else { ENC3_LED2 = LOW; }
    ENC3_LED3 = LOW; 
  } else if (v >= 340 && v < 425) { ENC3_LED1 = HIGH; ENC3_LED2 = HIGH; ENC3_LED3 = LOW; }
  else if (v >= 425 && v < 510) { 
    if (FLASH()) { ENC3_LED1 = HIGH; } else { ENC3_LED1 = LOW; }
    ENC3_LED2 = HIGH;
    ENC3_LED3 = LOW;
  } else if (v >= 510 && v < 595) { ENC3_LED1 = LOW; ENC3_LED2 = HIGH; ENC3_LED3 = LOW; }
  else if (v >= 595 && v < 680) { 
    ENC3_LED1 = LOW;
    ENC3_LED2 = HIGH;
    if (FLASH()) { ENC3_LED3 = HIGH; } else { ENC3_LED3 = LOW; }
  } else if (v >= 680 && v < 765) { ENC3_LED1 = LOW; ENC3_LED2 = HIGH; ENC3_LED3 = HIGH; }
  else if (v >= 765 && v < 850) { 
    ENC3_LED1 = LOW;
    if (FLASH()) { ENC3_LED2 = HIGH; } else { ENC3_LED2 = LOW; }
    ENC3_LED3 = HIGH;
  } else if (v >= 850 && v < 935) { ENC3_LED1 = LOW; ENC3_LED2 = LOW; ENC3_LED3 = HIGH; }
  else if (v >= 935) { 
    ENC3_LED1 = LOW;
    ENC3_LED2 = LOW;
    if (FLASH()) { ENC3_LED3 = HIGH; } else { ENC3_LED3 = LOW; }
  }
}

int t1 = 0;
int t2 = 0;
int ops = 0;
void printOps() {
  t1 = millis();
  if (t1 > t2) {
    t2 = t1 + 1000;
    Serial.print("Ops: "); Serial.println(ops);
    ops = 0;
  }
  ops += 1;
}

int processValue(int x) {
  // VALUES  VALUESinc
  VALUES[x] = VALUES[x] + VALUESinc[x];
  if (VALUES[x] < 0) {
    VALUES[x] = 0; VALUESinc[x] = 1;
  } else if (VALUES[x] > 1024) {
    VALUES[x] = 1024; VALUESinc[x] = -1;
  }
  return VALUES[x];
}

int incslower = 0; int enc1 = 0; int enc2 = 0; int enc3 = 0;

void loop() {
#ifndef DEBUG
  mb.task();
#endif
  if (incslower > 3) {
    enc1 = processValue(0);
    enc2 = processValue(1);
    enc3 = processValue(2);
#ifndef DEBUG
    mb.Hreg(RAM1_REG, processValue(3));
    mb.Hreg(RAM2_REG, processValue(4));
#else
    v1 = processValue(3);
    v2 = processValue(4);
#endif
    incslower = 0;
  } else {incslower++;}

#ifdef SERIALOUT
  Serial1.print("RAM1:"); Serial1.print(mb.Hreg(RAM1_REG)); Serial1.print("  RAM2:"); Serial1.println(mb.Hreg(RAM2_REG)); 
#endif
#ifdef MONITOR
  if (MONSTEP < 20) { MONSTEP++; }
  else {
    Serial3.print(enc1); Serial3.print(" "); Serial3.print(enc2); Serial3.print(" "); Serial3.print(enc3); Serial3.print(" "); 
#ifndef DEBUG
    Serial3.print(mb.Hreg(RAM1_REG)); Serial3.print(" "); Serial3.println(mb.Hreg(RAM2_REG));
#else
    Serial3.print(v1); Serial3.print(" "); Serial3.println(v2);
#endif
    MONSTEP = 0;
  }
#endif

#ifndef DEBUG
  //normal code  
  exDAC1.setVoltage(mb.Hreg(RAM1_REG), false);
  exDAC2.setVoltage(mb.Hreg(RAM2_REG), false);
#else
  // debug DAC code
  exDAC1.setVoltage(v1, false);
  exDAC2.setVoltage(v2, false);
#endif

  // LED update
  doENC1LEDs(enc1); doENC2LEDs(enc2); doENC3LEDs(enc3);
  
  // spinnaker
  if (spin_button.ischanged()) { spinnaker = !spinnaker; }
#ifdef DEBUG
  Serial.print("Spin: ");Serial.println(spinnaker);  
#else
  // set modbus data
  mb.Ists(SPIN_STATUS, spinnaker);
  mb.Ireg(MAINSH_REG, enc1);
  mb.Ireg(TILL_REG, enc2);
  mb.Ireg(HEEL_REG, enc3);
#endif
  //printOps
  FLASH_ITER();
}
