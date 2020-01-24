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
        bool isInitialized() const;
        bool remoteConnected() const;
        bool hubConnected() const;
        void setRemoteLed(Color color);
        void setHubLed(Color color);

    private:
        PoweredUpRemote _myRemote;
        PoweredUpHub _myHub;
        int _currentSpeed;
        int _requestedSpeed;
        bool _isInitialized;
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
        static const int Servo1Pin = 25;
        static const int Servo2Pin = 32;
        static const int Light1Pin = 21;
        static const int Light2Pin = 23;
        static const int LoopTime = 100;
        static const int TimeToShutdownSystem = 2000;
        static const int TimeForServoToActivateSwitch = 500;

        bool checkForConnections();
        static void IRAM_ATTR onTimer();

};

#if !defined(NO_GLOBAL_INSTANCES)
extern PoweredUpController PoweredUpLego;
#endif