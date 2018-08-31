#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 exDAC1;
Adafruit_MCP4725 exDAC2;

int v1 = 0;
int v2 = 4090;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Connected "); 
  exDAC1.begin(0x62);
  exDAC2.begin(0x63);
}

void loop() {
  // put your main code here, to run repeatedly:
  exDAC1.setVoltage(v1, false);
  exDAC2.setVoltage(v2, false);

  Serial.print("DAC 1: "); Serial.print(v1); 
  Serial.print("     DAC 2: "); Serial.println(v2);  
  
  if (v1 == 0) { v1 = 2048; }
  else { v1 = 0; }

  if (v2 == 0) { v2 = 4090; }
  else { v2 = 0; }
  
  delay(2000);
}
