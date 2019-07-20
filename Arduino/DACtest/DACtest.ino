//#define MONITOR
#include <Wire.h>
#include <Adafruit_MCP4725.h>


Adafruit_MCP4725 exDAC1;
Adafruit_MCP4725 exDAC2;

int v1 = 200;
int v2 = 200;
int VALUES[] = {0, 400, 800, 1200, 1600, 2000, 4095, 2048, 4095, 2048, 100, 4095, 500};
int LEN = 12;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Connected "); 
 #ifdef MONITOR
  Serial2.begin(9600);
 #endif
  exDAC1.begin(0x62);
  exDAC2.begin(0x63);
}

int i;

void loop() {
  // put your main code here, to run repeatedly:
  exDAC1.setVoltage(v1, false);
  exDAC2.setVoltage(v2, false);

  i++;
  if (i > LEN) { i = 0; }
#ifdef MONITOR
  Serial2.print(0); Serial2.print(" "); Serial2.print(0); Serial2.print(" "); Serial2.print(0); Serial2.print(" "); 
    Serial2.print(v1); Serial2.print(" "); Serial2.println(v2); 
#endif    
  Serial.print("DAC 1: "); Serial.print(v1); 
  Serial.print("     DAC 2: "); Serial.println(v2);  
  
  v1 = VALUES[i];
  v2 = VALUES[i];
  
  delay(2000);
}
