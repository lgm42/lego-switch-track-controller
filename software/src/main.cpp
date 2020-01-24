#include <Arduino.h>

#include "PoweredUpController.h"
#include "LedBlinking.h"

const int BatteryAnalogicPin = 27; 
const int RemoteLed = 18;
const int TrainLed = 33;
const int DeviceLed = 23;

const int WaitingDeviceBlinkingOnTime = 1;
const int WaitingDeviceBlinkingOffTime = 10;
const int ConnectedDeviceBlinkingOnTime = 5;
const int ConnectedDeviceBlinkingOffTime = 0;

const int BatteryOkBlinkingOnTime = 5;
const int BatteryOkBlinkingOffTime = 0;

const int BatteryLowBlinkingOnTime = 5;
const int BatteryLowBlinkingOffTime = 5;

const Color RealDeviceConnectedColor = BLUE;

hw_timer_t * _timer = NULL;
LedBlinking _remoteLed(WaitingDeviceBlinkingOnTime, WaitingDeviceBlinkingOffTime);
LedBlinking _hubLed(WaitingDeviceBlinkingOnTime, WaitingDeviceBlinkingOffTime);
LedBlinking _deviceLed(BatteryOkBlinkingOnTime, BatteryOkBlinkingOffTime);
LedBlinking _realRemoteLed(WaitingDeviceBlinkingOnTime, WaitingDeviceBlinkingOffTime);
LedBlinking _realHubLed(WaitingDeviceBlinkingOnTime, WaitingDeviceBlinkingOffTime);

void IRAM_ATTR onTimer() 
{
  _remoteLed.step();
  _hubLed.step();
  _deviceLed.step();
  _realRemoteLed.step();
  _realHubLed.step();

  //local leds
  digitalWrite(RemoteLed, !(_remoteLed.isLedOn())); //Led are driven when pin goes to low
  digitalWrite(TrainLed, !(_hubLed.isLedOn())); //Led are driven when pin goes to low
  digitalWrite(DeviceLed, !(_deviceLed.isLedOn())); //Led are driven when pin goes to low

  //leds on other devices
  if (PoweredUpLego.remoteConnected())
    PoweredUpLego.setRemoteLed(_realRemoteLed.isLedOn()?RealDeviceConnectedColor:BLACK);
  if (PoweredUpLego.hubConnected())
    PoweredUpLego.setHubLed(_realHubLed.isLedOn()?RealDeviceConnectedColor:BLACK);

  if (PoweredUpLego.remoteConnected())
    _remoteLed.configureBlink(ConnectedDeviceBlinkingOnTime, ConnectedDeviceBlinkingOffTime);
  else
    _remoteLed.configureBlink(WaitingDeviceBlinkingOnTime, WaitingDeviceBlinkingOffTime);
}

void setup() 
{
  Serial.begin(115200);
  while (!Serial) {} // wait for serial port to connect. Needed for native USB
  Serial.println("setup...");

  pinMode(RemoteLed, OUTPUT);
  digitalWrite(RemoteLed, 1);
  pinMode(TrainLed, OUTPUT);
  digitalWrite(TrainLed, 1);
  pinMode(DeviceLed, OUTPUT);
  digitalWrite(DeviceLed, 1);

  adcAttachPin(BatteryAnalogicPin);

  _timer = timerBegin(0, 80, true);
  timerAttachInterrupt(_timer, &onTimer, true);
  timerAlarmWrite(_timer, 100000, true);
  timerAlarmEnable(_timer);

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