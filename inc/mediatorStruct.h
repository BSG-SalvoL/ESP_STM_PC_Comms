#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "SimHubFeeder.h"
#include "vJoyFeeder.h"
#include "buttons.h"
//#include "SerialPort.hpp"
#include "config.h"

typedef struct Mediator {
    SimHubFeeder sh_feeder = SimHubFeeder::get_feeder();
    VJoyFeeder vJoyFeeder = VJoyFeeder::get_feeder();
    std::vector<Button*> buttons;
    void* gui;
    //SerialPort *Serial; //REMOVE THIS FROM SH FEEDER
    //SerialPort *Arduino; //REMOVE THIS FROM SH FEEDER
    uint64_t btn_state1 = 53453456342, btn_state2 = 3456467;
    Mediator() {
        srand(time(NULL));
        for (int i = 1; i <= VJOY_Conf.VJOY_BUTTONS; i++) {
            buttons.push_back(new Button(i, false));
        }
        gui = NULL;
    };
} Mediator; Mediator mediator;