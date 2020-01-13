#include <Arduino.h>

#include "PoweredUpController.h"

const int BatteryAnalogicPin = 22; 

void setup() 
{
  Serial.begin(115200);
  while (!Serial) {} // wait for serial port to connect. Needed for native USB
  Serial.println("setup...");

  PoweredUpLego.setup();

  Serial.println("setup finished.");
}

void loop() 
{
  PoweredUpLego.handle();
  int val = analogRead(BatteryAnalogicPin);
  Serial.println(val);
  delay(100);
}