// Initial example codes

#include <HID-Project.h>
#include <HID-Settings.h>

#define SIP 5
#define PUFF 6
#define SIPPLUS 4
#define PUFFPLUS 3

long XAXIS = 0;
long XACCEL = 0;
long YAXIS = 0;
long YACCEL = 0;

void setup() {
  pinMode(SIP, INPUT_PULLUP);
  pinMode(PUFF, INPUT_PULLUP);
  pinMode(SIPPLUS, INPUT_PULLUP);
  pinMode(PUFFPLUS, INPUT_PULLUP);
  delay(1);  // Give this transition time to "settle"
  // Get the initial state of the Opto-isolator output
  digitalRead(SIP);
  digitalRead(PUFF);
  digitalRead(SIPPLUS);
  digitalRead(PUFFPLUS);
  Serial.begin(9600);  
  Gamepad.begin();
}

void checkButton(int pin)
{
  if (digitalRead(pin))
      Gamepad.release(pin-8);
  else
    Gamepad.press(pin-8);
}

void loop() {
  if (digitalRead(SIP) && digitalRead(PUFF))
    XACCEL = 0;
  if (!digitalRead(SIP))
    XACCEL -= 15;
  if (!digitalRead(PUFF))
    XACCEL += 15;

  if (digitalRead(SIPPLUS) && digitalRead(PUFFPLUS))
    YACCEL = 0;  
  if (!digitalRead(SIPPLUS))
    YACCEL -= 15;
  if (!digitalRead(PUFFPLUS))
    YACCEL += 15;

  XAXIS += XACCEL;
  YAXIS += YACCEL;
  // Move x/y Axis to a new position (16bit)
  Gamepad.xAxis(XAXIS);
  Gamepad.yAxis(YAXIS);
  
  // Functions above only set the values.
  // This writes the report to the host.
  Gamepad.write();

  Serial.print("X: ");
  Serial.print(XAXIS);
  Serial.print(" XACC: ");
  Serial.print(XACCEL);
  Serial.print("Y: ");
  Serial.print(YAXIS);
  Serial.print("YACC: ");
  Serial.println(YACCEL);
  
  // Simple debounce
  delay(80);
}
