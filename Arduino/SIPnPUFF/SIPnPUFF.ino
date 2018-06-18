// Initial example codes

#include <HID-Project.h>
#include <HID-Settings.h>

#define SIP 5
#define PUFF 4
#define SIPPLUS 6
#define PUFFPLUS 3
#define RESETSW 2

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
  // Get the initial state of the Opto-isolator output
  digitalRead(SIP);
  digitalRead(PUFF);
  digitalRead(SIPPLUS);
  digitalRead(PUFFPLUS);
  Serial.begin(9600);  
  Gamepad.begin();
}

void loop() {
  if (digitalRead(SIP) && digitalRead(PUFF))
    XACCEL = 0;
  if (!digitalRead(SIP)) {
    if (XACCEL == 0) { XACCEL = -25;}
    XACCEL = floor(XACCEL * 1.4);
    XACCEL = max(-5000, XACCEL);
  }
  if (!digitalRead(PUFF)) {
    if (XACCEL == 0) { XACCEL = 25; }
    XACCEL = ceil(XACCEL * 1.4);
    XACCEL = min(5000, XACCEL);
  }

  if (digitalRead(SIPPLUS) && digitalRead(PUFFPLUS))
    YACCEL = 0;  
  if (!digitalRead(SIPPLUS)) {
    if (YACCEL == 0) { YACCEL = -25; }
    YACCEL = floor(YACCEL * 1.4);
    YACCEL = max(-5000, YACCEL);
  }
  if (!digitalRead(PUFFPLUS)) {
    if (YACCEL == 0) { YACCEL = 25; }
    YACCEL = ceil(YACCEL * 1.4);
    YACCEL = min(5000, YACCEL);
  }
  
  XAXIS += XACCEL;
  if (XAXIS >= 0) { XAXIS = min(32766, XAXIS);} 
  else { XAXIS = max(-32766, XAXIS); }
  YAXIS += YACCEL;
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
  Gamepad.xAxis(XAXIS);
  Gamepad.yAxis(YAXIS);
  
  // Functions above only set the values.
  // This writes the report to the host.
  Gamepad.write();

  Serial.print("X: ");
  Serial.print(XAXIS);
  Serial.print(" XACC: ");
  Serial.print(XACCEL);
  Serial.print("      Y: ");
  Serial.print(YAXIS);
  Serial.print("YACC: ");
  Serial.println(YACCEL);
  
  delay(20);
}
