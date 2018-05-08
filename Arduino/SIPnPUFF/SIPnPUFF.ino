// Initial example codes

#include <HID-Project.h>
#include <HID-Settings.h>

const int pinLed = LED_BUILTIN;

void setup() {
  pinMode(12,INPUT);
  pinMode(11,INPUT);
  pinMode(10,INPUT);
  pinMode(9,INPUT);
  pinMode(5,INPUT);
  
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
  digitalWrite(pinLed, HIGH);
  
  // press button
  checkButton(12);
  checkButton(11);
  checkButton(10);
  checkButton(9);
  Serial.print("Read: ");
  Serial.print(digitalRead(12));
  Serial.print(", ");
  Serial.print(digitalRead(11));
  Serial.print(", ");
  Serial.print(digitalRead(10));
  Serial.print(", ");
  Serial.println(digitalRead(9));
  if (digitalRead(11))
    digitalWrite(pinLed,1);
   else
    digitalWrite(pinLed,0);
  
  // Move x/y Axis to a new position (16bit)
  Gamepad.xAxis(random(0xFFFF));
  Gamepad.yAxis(random(0xFFFF));
  
  // Functions above only set the values.
  // This writes the report to the host.
  Gamepad.write();
  
  // Simple debounce
  delay(100);
  digitalWrite(pinLed, LOW);
}
