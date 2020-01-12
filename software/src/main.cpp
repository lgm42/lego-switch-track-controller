#include <Arduino.h>
#include <ArduinoOTA.h>

#include "ConfigurationProvider.h"
#include "WiFiManager.h"

#define NTP_SERVERS "0.fr.pool.ntp.org", "time.nist.gov", "pool.ntp.org"
#define UTC_OFFSET  +1

#define LED_BUILTIN 22

WiFiManager _wifiManager;

void setup() 
{
  Serial.begin(115200);
  while (!Serial) {} // wait for serial port to connect. Needed for native USB
  Serial.println("setup...");

  pinMode(LED_BUILTIN, OUTPUT);

  //_wifiManager.autoConnect(Configuration.parameters().hostname.c_str(), Configuration.parameters().hostname.c_str());
  _wifiManager.autoConnect("switch-track", "switch-track");
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(200);
      Serial.print(".");
  }
  Serial.println("WiFi-CONNECTED");

  configTime(UTC_OFFSET * 3600, 0, NTP_SERVERS);
  while (!time(nullptr)) {
    delay(50);
  }

  Configuration.setup();
  Configuration.loadFromFlash();

  ArduinoOTA.begin();

  Serial.println("setup finished.");
}

void loop() 
{
  ArduinoOTA.handle();
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}