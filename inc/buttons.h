#pragma once

#include <iostream>
#include <assert.h>

#include "config.h"

enum POV_Directions {
    POV_UP,
    POV_RIGHT,
    POV_DOWN,
    POV_LEFT
};

class Button {
    private:
        bool pressed;
        bool active;
        bool disabled;
        uint8_t bit_number;
    public:
        Button(uint8_t bit_number, bool is_disabled = false) {
            assert(bit_number > 0 && bit_number <= VJOY_Conf.VJOY_BUTTONS);

            this->bit_number = bit_number;
            this->disabled = is_disabled;
            if(is_disabled == true)
                active = false;
            if(is_disabled == false)
                active = true;
            pressed = false;
        }
        bool& is_pressed(uint64_t buttons) {
            if(buttons & 1 << (bit_number-1) > 0)
                this->pressed = true;
            else
                this->pressed = false;
            return pressed;
        }
        
        bool& is_active() {
            return active;
        }
        bool& is_disabled() {
            return disabled;
        }

        void change_active_status(bool is_disabled) {
            if(is_disabled) {
                this->disabled = true;
                this->active = false;
            }
            else {
                this->active = true;
                this->disabled = false;
            }
        }
};