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
// https://github.com/Clam-/modbus-arduino Modified from https://github.com/epsilonrt/modbus-arduino

#include <Adafruit_MCP4725.h>

#include "GPIO.h" // https://github.com/mikaelpatel/Arduino-GPIO
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
const int MAINSH_REG  = 642; // 40643
const int TILL_REG    = 643; // 40644
const int HEEL_REG    = 644; // 40645
#ifndef DEBUG
ModbusSerial mb;
#endif

const unsigned int ENCEXT = B100000;
unsigned int OPS = 0;
unsigned int PREVOPS = 0;
unsigned long PREVOPSTIME = 0;
unsigned long PREVMONTIME = 0;
unsigned long MONFREQ = 50;
unsigned long OPSFREQ = 1000;
unsigned long DELAYPREV = 0;
unsigned long DELAYLENGTH = 1;
unsigned long RAMCOOLTIME = 0;
bool RAMON = true;
unsigned long RAMCOOLDURATION = 1500;
unsigned int ENC1EXT = ENCEXT;
unsigned int ENC2EXT = ENCEXT;
unsigned int ENC3EXT = ENCEXT;

// #################################
// PIN CONFIGURATION
// #################################
GPIO<BOARD::D12> SPIN_BUTTON; // Spinnaker

// Encoder 1  MainSheet
GPIO<BOARD::D4>     ENC1_CS;      // Blue
GPIO<BOARD::D3>     ENC1_DATA;    // Orange
GPIO<BOARD::D2>     ENC1_CLOCK;   // Yellow
GPIO<BOARD::D40>     ENC1_LED1;
GPIO<BOARD::D38>     ENC1_LED2;
GPIO<BOARD::D36>     ENC1_LED3;

// Encoder 2  Rudder
GPIO<BOARD::D7>     ENC2_CS;      // Blue
GPIO<BOARD::D6>     ENC2_DATA;    // Orange
GPIO<BOARD::D5>     ENC2_CLOCK;   // Yellow
GPIO<BOARD::D34>     ENC2_LED1;
GPIO<BOARD::D32>     ENC2_LED2;
GPIO<BOARD::D30>     ENC2_LED3;

// Encoder 3  Heel
GPIO<BOARD::D10>    ENC3_CS;      // Blue
GPIO<BOARD::D9>     ENC3_DATA;    // Orange
GPIO<BOARD::D8>     ENC3_CLOCK;   // Yellow
GPIO<BOARD::D28>     ENC3_LED1;
GPIO<BOARD::D26>     ENC3_LED2;
GPIO<BOARD::D24>     ENC3_LED3;

GPIO<BOARD::D13>     TEST_LED;

void initEnc() {
  // Set mode of encoder pins
  ENC1_CS.output(); ENC2_CS.output(); ENC3_CS.output();
  ENC1_CLOCK.output(); ENC2_CLOCK.output(); ENC3_CLOCK.output();
  ENC1_DATA.input(); ENC2_DATA.input(); ENC3_DATA.input();

  // Set Encoder initial states
  ENC1_CLOCK = HIGH; ENC2_CLOCK = HIGH; ENC3_CLOCK = HIGH;
  ENC1_CS = LOW;     ENC2_CS = LOW;     ENC3_CS = LOW;
}

void initLEDs() {
  // Set LED pin modes
  ENC1_LED1.output(); ENC1_LED2.output(); ENC1_LED3.output();
  ENC2_LED1.output(); ENC2_LED2.output(); ENC2_LED3.output();
  ENC3_LED1.output(); ENC3_LED2.output(); ENC3_LED3.output();
  ENC1_LED1 = LOW; ENC1_LED2 = LOW; ENC1_LED3 = LOW;
  ENC2_LED1 = LOW; ENC2_LED2 = LOW; ENC2_LED3 = LOW;
  ENC3_LED1 = LOW; ENC3_LED2 = LOW; ENC3_LED3 = LOW;
}

void setup() {
  Wire.setClock(400000);
#ifdef SERIALOUT
  Serial1.begin(9600);
  Serial1.println("Connected ");
#endif
#ifdef MONITOR
  Serial.begin(57600);
  //Serial.println("Opened Serial3.");
#endif

#ifndef DEBUG
  mb.config(&SerialUSB, 38400); //SERIAL_8N1
  mb.setSlaveId(SLAVE_ID);
  mb.addIsts(SPIN_STATUS, false);
  mb.addHreg(RAM1_REG); mb.addHreg(RAM2_REG);
  mb.addIreg(MAINSH_REG); mb.addIreg(TILL_REG); mb.addIreg(HEEL_REG);
  mb.addIreg(0); mb.addIreg(1);
#endif
  // For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
  // For MCP4725A0 the address is 0x60 or 0x61
  // For MCP4725A2 the address is 0x64 or 0x65
  exDAC1.begin(0x62); exDAC2.begin(0x63);
#ifdef FIRSTTIMESETUP
  exDAC1.setVoltage(0, true); exDAC2.setVoltage(0, true);
#else
  exDAC1.setVoltage(0, false); exDAC2.setVoltage(0, false);
#endif
  initEnc();

  initLEDs();
  SPIN_BUTTON.input();
  TEST_LED.output();
  PREVOPSTIME = PREVMONTIME = DELAYPREV = millis();
}

unsigned int readEncoder1(unsigned int pos, int cycle, int ibit) {
  if (cycle == 0) { ENC1_CS = HIGH; }
  else if (cycle == 1) { ENC1_CS = LOW; pos = 0; }
  // fetch one bit
  else if (cycle >= 2 && cycle < 18) {
    if (ibit == 0) { ENC1_CLOCK = LOW; }
    else if (ibit == 1) { ENC1_CLOCK = HIGH; }
    else if (ibit == 2) {
      pos = pos | ENC1_DATA;
      if (cycle<17) { pos = pos << 1; } // shift every bit except last one.
    }
  }
  // trailing clock
  else if (cycle == 18) { ENC1_CLOCK = LOW; }
  else if (cycle == 19) { ENC1_CLOCK = HIGH; }
  return pos; // including extdata
}
unsigned int readEncoder2(unsigned int pos, int cycle, int ibit) {
  if (cycle == 0) { ENC2_CS = HIGH; }
  else if (cycle == 1) { ENC2_CS = LOW; pos = 0; }
  // fetch one bit
  else if (cycle >= 2 && cycle < 18) {
    if (ibit == 0) { ENC2_CLOCK = LOW; }
    else if (ibit == 1) { ENC2_CLOCK = HIGH; }
    else if (ibit == 2) {
      pos = pos | ENC2_DATA;
      if (cycle<17) { pos = pos << 1; } // shift every bit except last one.
    }
  }
  // trailing clock
  else if (cycle == 18) { ENC2_CLOCK = LOW; }
  else if (cycle == 19) { ENC2_CLOCK = HIGH; }
  return pos; // including extdata
}
unsigned int readEncoder3(unsigned int pos, int cycle, int ibit) {
  if (cycle == 0) { ENC3_CS = HIGH; }
  else if (cycle == 1) { ENC3_CS = LOW; pos = 0; }
  // fetch one bit
  else if (cycle >= 2 && cycle < 18) {
    if (ibit == 0) { ENC3_CLOCK = LOW; }
    else if (ibit == 1) { ENC3_CLOCK = HIGH; }
    else if (ibit == 2) {
      pos = pos | ENC3_DATA;
      if (cycle<17) { pos = pos << 1; } // shift every bit except last one.
    }
  }
  // trailing clock
  else if (cycle == 18) { ENC3_CLOCK = LOW; }
  else if (cycle == 19) { ENC3_CLOCK = HIGH; }
  return pos; // including extdata
}

// If this returns anything other than 0, there's parity error.
unsigned int parity_check(unsigned int v){
  //http://graphics.stanford.edu/~seander/bithacks.html#ParityNaive
  v ^= v >> 16;
  v ^= v >> 8;
  v ^= v >> 4;
  v &= 0xf;
  return (0x6996 >> v) & 1;
}

void monitorData(unsigned long millist, unsigned int e1, unsigned int e2, unsigned int e3) {
  // check encoder extra bits.
  unsigned int enc1e = e1 & B011110;
  unsigned int enc2e = e2 & B011110;
  unsigned int enc3e = e3 & B011110;
  if (parity_check(e1)) { enc1e | 1; }
  if (parity_check(e2)) { enc2e | 1; }
  if (parity_check(e3)) { enc3e | 1; }
  // add bit to error persistence
  if (enc1e != ENCEXT) { ENC1EXT = enc1e | ENC1EXT; }
  if (enc2e != ENCEXT) { ENC2EXT = enc2e | ENC2EXT; }
  if (enc3e != ENCEXT) { ENC3EXT = enc3e | ENC3EXT; }

  if (((millist - PREVMONTIME) > MONFREQ) && (Serial.availableForWrite() > 120)) {
    // Debug format: enc1  enc2  enc3  ram1  ram2  ops ext1 ext2 ext3
    //               BBBB BBBB BBBB BBBB BBBB OOO EE EE EE
      Serial.print(e1, HEX); Serial.print(" "); Serial.print(e2, HEX); Serial.print(" "); Serial.print(e3, HEX); Serial.print(" ");
      Serial.print(mb.Hreg(RAM1_REG), HEX); Serial.print(" "); Serial.print(mb.Hreg(RAM2_REG), HEX); Serial.print(" ");
      Serial.print(PREVOPS, HEX); Serial.print(" "); Serial.print(ENC1EXT, HEX); Serial.print(" "); Serial.print(ENC2EXT, HEX);
      Serial.print(" "); Serial.println(ENC3EXT, HEX);
      PREVMONTIME = millist;
    }
  }
}

unsigned int STAGEITER = 0; // Used to cycle though each encoder bit
int ENCBITITER = 0; // Used to keep track of which phase of the bit fetch we are in (clock, or read)
bool WARMUP = true;
unsigned int ENC1 = 0, ENC2 = 0, ENC3 = 0;

void loop() {
  unsigned long millist = millis();
  unsigned int ram1 = 0, ram2 = 0, ram1p = 0, ram2p = 0;
  // Process Modbus Comms.
  mb.task();

  // RAM Control
  ram1 = mb.Hreg(RAM1_REG);
  ram2 = mb.Hreg(RAM2_REG);
  if (ram1 == 22 && ram2 == 22) {
    // lockout RAM for 1s
    RAMON = false;
    exDAC1.setVoltage(0, false); exDAC2.setVoltage(0, false);
    RAMCOOLTIME = millist;
  }
  if (RAMON) {
    exDAC1.setVoltage(ram1, false); exDAC2.setVoltage(ram2, false);
  } else {
    if (millist - RAMCOOLTIME > RAMCOOLDURATION){
      RAMON = true;
    }
    exDAC1.setVoltage(0, false); exDAC2.setVoltage(0, false);
    mb.Hreg(RAM1_REG, 0); mb.Hreg(RAM2_REG, 0);
  }

  // encoder/spinnaker read cycle (with delay)
  if (true || millist - DELAYPREV > DELAYLENGTH) {
    //DELAYPREV = millis();
    if (STAGEITER == 0) { mb.Ists(SPIN_STATUS, !SPIN_BUTTON); } // Spinnaker update

    ENC1 = readEncoder1(ENC1, STAGEITER, ENCBITITER);
    ENC2 = readEncoder2(ENC2, STAGEITER, ENCBITITER);
    ENC3 = readEncoder3(ENC3, STAGEITER, ENCBITITER);

    if (STAGEITER < 2 || (STAGEITER >= 17 && STAGEITER < 20)) { STAGEITER++; } // Increment stage if not on 2
    else if (STAGEITER >= 2 && STAGEITER < 17) {
      // If stage 2-16 we want to cycle through the bitphase before going to next stage.
      ENCBITITER++;
      if (ENCBITITER > 2) { STAGEITER++; ENCBITITER = 0; }
    }
    else if (STAGEITER == 20) {
       // end of cycle
       if (!WARMUP) {
        monitorData(millist, ENC1, ENC2, ENC3); // output monitor data
        // set encoder data to modbus registers
        mb.Ireg(MAINSH_REG, ENC1 >> 6);
        mb.Ireg(TILL_REG, ENC2 >> 6);
        mb.Ireg(HEEL_REG, ENC3 >> 6);
        OPS++;
       }
       STAGEITER = 0; WARMUP = false; ENCBITITER = 0;
    }
  }
  //ops calc
  if (millist - PREVOPSTIME > OPSFREQ){
    PREVOPS = OPS; OPS = 0;
    PREVOPSTIME = millist;
  }
}
