#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif
 
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

#include "BluefruitConfig.h"

#define VERBOSE_MODE                   true  // If set to 'true' enables debug output
 
// Create the bluefruit object
 
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}
 
// The service and characteristic index information 
int32_t gattServiceId;
int32_t gattNotifiableCharId;
int32_t gattWritableResponseCharId;
int32_t gattWritableNoResponseCharId;
int32_t gattReadableCharId;
 
void setup(void)
{
  //remove these 2 lines if not debugging - nothing will start until you open the serial window
  while (!Serial); // required for Flora & Micro
  delay(500);
 
  boolean success;
 
  Serial.begin(115200);
  Serial.println(F("Adafruit Custom GATT Service Example"));
  Serial.println(F("---------------------------------------------------"));
 
  randomSeed(micros());
 
  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));
 
  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );
 
  /* Perform a factory reset to make sure everything is in a known state */
  Serial.println(F("Performing a factory reset: "));
  if (! ble.factoryReset() ){
       error(F("Couldn't factory reset"));
  }
 
  /* Disable command echo from Bluefruit */
  ble.echo(false);
 
  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();
 
  // this line is particularly required for Flora, but is a good idea
  // anyways for the super long lines ahead!
//  ble.setInterCharWriteDelay(5); // 5 ms
 
 
  /* Add the Custom GATT Service definition */
  /* Service ID should be 1 */
  Serial.println(F("Adding the Custom GATT Service definition: "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDSERVICE=UUID128=8F-0A-2D-D3-EA-0F-43-CB-A5-B4-3F-52-9F-91-D9-1E"), &gattServiceId);
  if (! success) {
    error(F("Could not add Custom GATT service"));
  }
 
  /* Add the Readable/Notifiable characteristic - */
  /* Characteristic ID should be 1 */
  Serial.println(F("Adding the Notifiable characteristic: "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID128=4A-A5-D0-B1-D4-DF-40-11-A1-84-13-C8-4E-8C-74-04,PROPERTIES=0x12,MIN_LEN=1, MAX_LEN=18, VALUE=55, DATATYPE=3, DESCRIPTION=sensor"), &gattNotifiableCharId);
    if (! success) {
    error(F("Could not add Custom Notifiable characteristic"));
  }

   Serial.println(F("Adding the Notifiable characteristic 2: "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID128=EE-2E-A7-46-18-7E-41-B2-B9-98-71-8A-9C-3F-7A-8B,PROPERTIES=0x12,MIN_LEN=1, MAX_LEN=18, VALUE=55, DATATYPE=3, DESCRIPTION=sensor2"), &gattNotifiableCharId);
    if (! success) {
    error(F("Could not add Custom Notifiable characteristic"));
  }
  /* Add the Custom GATT Service to the advertising data */
  //0x2DD3 from AT+GATTLIST - 16 bit svc id (reverse, starting at 3rd byte of service UUID)
  Serial.print(F("Adding Custom GATT Service UUID to the advertising payload: "));
  ble.sendCommandCheckOK( F("AT+GAPSETADVDATA=02-01-06-03-02-D3-2D") );
 
  /* Reset the device for the new service setting changes to take effect */
  Serial.print(F("Performing a SW reset (service changes require a reset): "));
  ble.reset();
 
  Serial.println();
}
 
void loop(void)
{
 
}
