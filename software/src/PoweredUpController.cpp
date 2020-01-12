
#include "PoweredUpController.h"

PoweredUpController::PoweredUpController() : _currentSpeed(0), _requestedSpeed(0), _isInitialized(false), _timer(NULL),
_currentServo1Position(0), _requestedServo1Position(0), _currentServo2Position(0), _requestedServo2Position(0), _waitForButtonRelease(false)
{
}

PoweredUpController::~PoweredUpController()
{
}

void PoweredUpController::setup()
{
    
}

void PoweredUpController::handle()
{
    
}

#if !defined(NO_GLOBAL_INSTANCES)
PoweredUpController PoweredUpLego;
#endif