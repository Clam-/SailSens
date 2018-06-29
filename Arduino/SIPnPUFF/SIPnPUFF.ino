// Requires HID library from https://github.com/NicoHood/HID

#include <HID-Project.h>
#include <HID-Settings.h>

// IO Pins for each action
#define SIP 5
#define PUFF 4
#define SIPPLUS 6
#define PUFFPLUS 3
#define RESETSW 2

// Somewhat sane limits and rates
#define MAXACCEL 1000
#define MAXDECEL -1000
#define ACCELRATE 1.1

// State
long XAXIS = 0;
long XACCEL = 0;
long YAXIS = 0;
long YACCEL = 0;

void setup() {
  pinMode(SIP, INPUT_PULLUP);
  pinMode(PUFF, INPUT_PULLUP);
  pinMode(SIPPLUS, INPUT_PULLUP);
  pinMode(PUFFPLUS, INPUT_PULLUP);
  pinMode(RESETSW, INPUT_PULLUP);
  
  delay(10);  // Give this transition time to "settle"
  // Get the initial state of the Opto-isolator outputs
  digitalRead(SIP);
  digitalRead(PUFF);
  digitalRead(SIPPLUS);
  digitalRead(PUFFPLUS);
  Serial.begin(9600);  
  Gamepad.begin();
}

void loop() {
  // Process X Axis (Sip and Puff) Acceleration
  if (digitalRead(SIP) && digitalRead(PUFF))
    XACCEL = 0;
  if (!digitalRead(SIP)) {
    if (XACCEL == 0) { XACCEL = -25;}
    XACCEL = floor(XACCEL * ACCELRATE);
    XACCEL = max(MAXDECEL, XACCEL);
  }
  if (!digitalRead(PUFF)) {
    if (XACCEL == 0) { XACCEL = 25; }
    XACCEL = ceil(XACCEL * ACCELRATE);
    XACCEL = min(MAXACCEL, XACCEL);
  }

  // Process Y Axis (Sip Plus and Puff Plus) Acceleration
  if (digitalRead(SIPPLUS) && digitalRead(PUFFPLUS))
    YACCEL = 0;  
  if (!digitalRead(SIPPLUS)) {
    if (YACCEL == 0) { YACCEL = -25; }
    YACCEL = floor(YACCEL * ACCELRATE);
    YACCEL = max(MAXDECEL, YACCEL);
  }
  if (!digitalRead(PUFFPLUS)) {
    if (YACCEL == 0) { YACCEL = 25; }
    YACCEL = ceil(YACCEL * ACCELRATE);
    YACCEL = min(MAXACCEL, YACCEL);
  }

  // Update Axis state
  XAXIS += XACCEL;
  // Make sure not to overflow USB Gamepad Axis
  if (XAXIS >= 0) { XAXIS = min(32766, XAXIS);} 
  else { XAXIS = max(-32766, XAXIS); }
    YAXIS += YACCEL;
  // Make sure not to overflow USB Gamepad Axis
  if (YAXIS >= 0) { YAXIS = min(32766, YAXIS);} 
  else { YAXIS = max(-32766, YAXIS); }
  // Move x/y Axis to a new position (16bit)

  //Handel Reset Switch
  if (!digitalRead(RESETSW)) {
    XACCEL = 0;
    YACCEL = 0;
    XAXIS = 0;
    YAXIS = 0;
    return;
  }
  // Set position for USB Host
  Gamepad.xAxis(XAXIS);
  Gamepad.yAxis(YAXIS);
  
  // Functions above only set the values.
  // This writes the report to the host.
  Gamepad.write();

  // USB Serial debugging
  Serial.print("X: ");
  Serial.print(XAXIS);
  Serial.print(" XACC: ");
  Serial.print(XACCEL);
  Serial.print("      Y: ");
  Serial.print(YAXIS);
  Serial.print("YACC: ");
  Serial.println(YACCEL);
  
  delay(20); // Could perhaps do without delay, but then acceleration rate value may need to be too low.
}
