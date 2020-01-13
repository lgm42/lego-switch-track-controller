
#include "PoweredUpController.h"

#define LED_BUILTIN 22

void IRAM_ATTR PoweredUpController::onTimer() 
{
  if (PoweredUpLego._isInitialized) 
  {
    //led management
    if (!PoweredUpLego._myRemote.isLeftRemoteUpButtonPressed() && !PoweredUpLego._myRemote.isLeftRemoteDownButtonPressed() &&
        !PoweredUpLego._myRemote.isRightRemoteUpButtonPressed() && !PoweredUpLego._myRemote.isRightRemoteDownButtonPressed() && 
        !PoweredUpLego._myRemote.isLeftRemoteStopButtonPressed() && !PoweredUpLego._myRemote.isRightRemoteStopButtonPressed())
    {
      //normal blinking
      if (PoweredUpLego._blink)
      {
        PoweredUpLego._requestedRemoteColor = PURPLE; 
        PoweredUpLego._requestedTrainColor = PURPLE;
        digitalWrite(LED_BUILTIN, 1);
      }
      else
      {
        PoweredUpLego._requestedRemoteColor = BLACK;
        PoweredUpLego._requestedTrainColor = BLACK; 
        digitalWrite(LED_BUILTIN, 0);
      }
      PoweredUpLego._blink = !PoweredUpLego._blink;   
    }
  }
  else
  {
      
  }
}

PoweredUpController::PoweredUpController() : _currentSpeed(0), _requestedSpeed(0), _isInitialized(false), _timer(NULL),
_currentServo1Position(0), _requestedServo1Position(0), _currentServo2Position(0), _requestedServo2Position(0), _waitForButtonRelease(false),
_blink(false)
{
}

PoweredUpController::~PoweredUpController()
{
}

void PoweredUpController::setup()
{
    _requestedServo1Position = SwitchLeftServoPosition;
    _requestedServo2Position = SwitchLeftServoPosition;
    _myRemote.init(); // initalize the listening hub
    _myHub.init(); // initalize the listening hub

    _timer = timerBegin(0, 80, true);
    timerAttachInterrupt(_timer, &PoweredUpController::onTimer, true);
    timerAlarmWrite(_timer, 1000000, true);
    timerAlarmEnable(_timer);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, 1);
    _onOffButtonCounter = 0;
}

bool PoweredUpController::isInitialized() const
{
    return _isInitialized;
}

bool PoweredUpController::checkForConnections()
{
    //wait for two elements
    if (_myRemote.isConnecting())
    {
        if (_myRemote.getHubType() == POWERED_UP_REMOTE)
        {
            //This is the right device 
            if (!_myRemote.connectHub())
            {
                Serial.println("Unable to connect to hub");
            }
            else
            {
                _myRemote.setLedColor(GREEN);
                Serial.println("Remote connected.");
            }
        }
    }

    if (_myHub.isConnecting())
    {
        if (_myHub.getHubType() == POWERED_UP_HUB)
        {
            _myHub.connectHub();
            _myHub.setLedColor(GREEN);
            Serial.println("powered up hub connected.");
        }
    }

    if (!_myRemote.isConnected())
    {
        _myRemote.init();
    }

    if (! _myHub.isConnected())
    {
        _myHub.init();
    }

    //everybody connected just now
    if (_myRemote.isConnected() && _myHub.isConnected() && !_isInitialized)
    {
        Serial.println("System is initialized");
        _isInitialized = true;

        // both activations are needed to get status updates
        _myRemote.activateButtonReports(); 
        _myRemote.activatePortDevice(PoweredUpRemote::Port::LEFT, 55);
        _myRemote.activatePortDevice(PoweredUpRemote::Port::RIGHT, 55);
    }

    return _isInitialized;
}

void PoweredUpController::handle()
{
    if (checkForConnections())
    {
        //led management
        if (_myRemote.isLeftRemoteUpButtonPressed() || _myRemote.isLeftRemoteDownButtonPressed())
        {
            _requestedRemoteColor = GREEN;
        }
        else if (_myRemote.isRightRemoteUpButtonPressed() || _myRemote.isRightRemoteDownButtonPressed())
        {
            _requestedRemoteColor = BLUE;
        }
        else if (_myRemote.isLeftRemoteStopButtonPressed())
        {
            _requestedRemoteColor = RED;
        }
        else if (_myRemote.isButtonPressed()) //Green button
        {
            _requestedRemoteColor = RED;
            //TODO : remove this shutdown when ON/OFF button is okay
            _onOffButtonCounter++;
            if (_onOffButtonCounter * LoopTime > TimeToShutdownSystem)
            {
                _myRemote.setLedColor(BLACK);
                _myRemote.shutDownHub();
                _myHub.shutDownHub();
                _isInitialized = false;
                while (true);
            }
        }
        else
        {
            _onOffButtonCounter = 0;
        }

        //Serial.printf("Current speed: %d\n", _currentSpeed);

        if (_waitForButtonRelease)
        {
            //we just wait for the release of all button to take an other into account
            if ((! _myRemote.isLeftRemoteUpButtonPressed()) && (! _myRemote.isLeftRemoteDownButtonPressed()) && 
                (! _myRemote.isLeftRemoteStopButtonPressed()) && 
                (! _myRemote.isRightRemoteUpButtonPressed()) && (! _myRemote.isRightRemoteDownButtonPressed()) && 
                (! _myRemote.isRightRemoteStopButtonPressed()))
            {
                //all right buttons released
                _waitForButtonRelease = false;
            }
        }
        else
        {
            //train management
            if (_myRemote.isLeftRemoteUpButtonPressed()) 
            {
                _requestedSpeed = min(100, _currentSpeed+10);
                _waitForButtonRelease = true;
            } 
            else if (_myRemote.isLeftRemoteDownButtonPressed()) 
            {
                _requestedSpeed = min(100, _currentSpeed-10);
                _waitForButtonRelease = true;
            } 
            else if (_myRemote.isLeftRemoteStopButtonPressed()) 
            {
                _requestedSpeed = 0;
                _waitForButtonRelease = true;
            } 

            //train switch management
            if (_myRemote.isRightRemoteUpButtonPressed())
            {
                if (_currentServo1Position == SwitchRightServoPosition)
                    _requestedServo1Position = SwitchLeftServoPosition;
                else
                    _requestedServo1Position = SwitchRightServoPosition;
                _waitForButtonRelease = true;
            }
            else if (_myRemote.isRightRemoteDownButtonPressed())
            {
                if (_currentServo2Position == SwitchRightServoPosition)
                    _requestedServo2Position = SwitchLeftServoPosition;
                else
                    _requestedServo2Position = SwitchRightServoPosition;
                _waitForButtonRelease = true;
            }
        }

        //apply requested values
        if (_requestedRemoteColor != _currentRemoteColor)
        {
            _myRemote.setLedColor(_requestedRemoteColor);
            _currentRemoteColor = _requestedRemoteColor;
        }

        if (_requestedTrainColor != _currentTrainColor)
        {
            _myHub.setLedColor(_requestedTrainColor);
            _currentTrainColor = _requestedTrainColor;
        }

        if (_currentSpeed != _requestedSpeed) 
        {
            //step of +2 or -2 depending on the direction to take
            int step = 2 * ((_requestedSpeed - _currentSpeed) / abs(_requestedSpeed - _currentSpeed));
            _currentSpeed += step;
            _myHub.setMotorSpeed(PoweredUpHub::Port::A, _currentSpeed);
            Serial.printf("Setting speed to %d\n", _currentSpeed);
        }

        if (_requestedServo1Position != _currentServo1Position)
        {
            Serial.printf("Driving Switch 1 to %d\n", _requestedServo1Position);
            _servo.attach(Servo1Pin);
            _servo.write(_requestedServo1Position);
            delay(TimeForServoToActivateSwitch);
            _servo.detach();
            _currentServo1Position = _requestedServo1Position;
            Serial.printf("Switch 1 driven\n");
        }

        if (_requestedServo2Position != _currentServo2Position)
        {
            Serial.printf("Driving Switch 2 to %d\n", _requestedServo2Position);
            _servo.attach(Servo2Pin);
            _servo.write(_requestedServo2Position);
            delay(TimeForServoToActivateSwitch);
            _servo.detach();
            _currentServo2Position = _requestedServo2Position;
            Serial.printf("Switch 2 driven\n");
        }
    }
}

#if !defined(NO_GLOBAL_INSTANCES)
PoweredUpController PoweredUpLego;
#endif