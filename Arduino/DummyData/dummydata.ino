/*********************************************************************

*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include <String.h>

//#include "BluefruitConfig.h"

// Create the bluefruit object, either software serial...uncomment these lines
/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Dummy Data Setup..."));
  Serial.println(F("-------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();
}

int counter=1;
/**************************************************************************/
/*!
    @brief  loop
*/
/**************************************************************************/
void loop(void)
{ 
  Serial.print(F("Value: "));
  Serial.println(counter);
  String val = String(counter);
  String senid = String(counter % 2 == 0 ? "1" : "2");
  String cmd = String("AT+GATTCHAR=" + senid + ", " + val);
  ble.println(cmd);
  if ( !ble.waitForOK() )
  {
    error(F("Failed to get response from notify property update"));
  }

//  ble.println("AT+GATTCHAR=1");
//  ble.readline();
//  if (strcmp(ble.buffer, "OK") != 0) {
//    // Some data was found, its in the buffer
//    Serial.print(F("[Recv] ")); 
//    Serial.println(ble.buffer);
//  }
  
  delay(500);
  counter++;
}

