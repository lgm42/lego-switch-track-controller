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


};

#if !defined(NO_GLOBAL_INSTANCES)
extern PoweredUpController PoweredUpLego;
#endif