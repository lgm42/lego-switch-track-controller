#pragma once

#include <Arduino.h>
#include "poweredUp/PoweredUpRemote.h"
#include "poweredUp/PoweredUpHub.h"

#include "Servo.h"

class PoweredUpController
{
    public:
        PoweredUpController();
        virtual ~PoweredUpController();

        void setup();
        void handle();

    private:
        PoweredUpRemote _myRemote;
        PoweredUpHub _myHub;
        int _currentSpeed;
        int _requestedSpeed;
        bool _isInitialized;
        hw_timer_t * _timer;
        Servo _servo;

        int _currentServo1Position;
        int _requestedServo1Position;
        int _currentServo2Position;
        int _requestedServo2Position;
        bool _waitForButtonRelease;
        int _onOffButtonCounter;

        Color _currentRemoteColor;
        Color _requestedRemoteColor;
        Color _currentTrainColor;
        Color _requestedTrainColor;

        static const int SwitchLeftServoPosition = 40;
        static const int SwitchRightServoPosition = 95;
        static const int Servo1Pin = 4;
        static const int Servo2Pin = 5;

};

#if !defined(NO_GLOBAL_INSTANCES)
extern PoweredUpController PoweredUpLego;
#endif