#pragma once

#include <Arduino.h>
#include "poweredUp/PoweredUpRemote.h"
#include "poweredUp/PoweredUpHub.h"

#include "Servo.h"

class LedBlinking
{
    public :
    LedBlinking(const int stepsOn = 1, const int stepsOff = 1)
        : _stepsOn(stepsOn), _stepsOff(stepsOff), _currentStep(0)
    {
    }

    virtual ~LedBlinking()
    {
    }

    void step()
    {
        _currentStep++;
        if (_currentStep >= _stepsOn + _stepsOff)
            _currentStep = 0;
        
    }

    bool isLedOn() const
    {
        //Led is on only is the stepsOn havn't been passed
        return (_currentStep < _stepsOn);
    }

    void configureBlink(const int stepsOn = 1, const int stepsOff = 1)
    {
        _stepsOn = stepsOn;
        _stepsOff = stepsOff;
        //assuming that _currentStep could be too high
        //not resetted to manage periodic configureBlink calls
    }

    private:
        int _stepsOn;
        int _stepsOff;
        int _currentStep;
};