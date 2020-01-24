
#include "PoweredUpController.h"

PoweredUpController::PoweredUpController() : _currentSpeed(0), _requestedSpeed(0), _isInitialized(false),
_currentServo1Position(0), _requestedServo1Position(0), _currentServo2Position(0), _requestedServo2Position(0), _waitForButtonRelease(false)
{
}

PoweredUpController::~PoweredUpController()
{
}

void PoweredUpController::setup()
{
    Serial.println("PoweredUpController setup ...");
    _requestedServo1Position = SwitchLeftServoPosition;
    _requestedServo2Position = SwitchLeftServoPosition;

    _myRemote.init(); // initalize the listening hub
    _myHub.init(); // initalize the listening hub

    _onOffButtonCounter = 0;
}

bool PoweredUpController::isInitialized() const
{
    return _isInitialized;
}

bool PoweredUpController::remoteConnected() const
{
    return _myRemote.isConnected();
}

bool PoweredUpController::hubConnected() const
{
    return _myHub.isConnected();
}

void PoweredUpController::setRemoteLed(Color color)
{
    _requestedRemoteColor = color;
}

void PoweredUpController::setHubLed(Color color)
{
    _requestedTrainColor = color;
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
                Serial.println("Remote connected.");
            }
        }
    }

    if (_myHub.isConnecting())
    {
        if (_myHub.getHubType() == POWERED_UP_HUB)
        {
            _myHub.connectHub();
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
        if (_myRemote.isButtonPressed()) //Green button
        {
            _onOffButtonCounter++;
            if (_onOffButtonCounter * LoopTime > TimeToShutdownSystem)
            {
                _myHub.setLedColor(BLACK);
                _myHub.shutDownHub();
                _myRemote.setLedColor(BLACK);
                _myRemote.shutDownHub();
                _isInitialized = false;
                _myHub = PoweredUpHub();
                _myRemote = PoweredUpRemote();
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