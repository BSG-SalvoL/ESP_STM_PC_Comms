#pragma once

#include <iostream>

typedef struct VJOY_CONF {
    uint8_t VJOY_DEVICE_ID;
    uint16_t VJOY_BUTTONS;
    bool IS_FFB;

    VJOY_CONF() : VJOY_DEVICE_ID(1), VJOY_BUTTONS(128), IS_FFB(false) {}
} VJOY_CONF; VJOY_CONF VJOY_Conf;

typedef struct SIMHUB_CONF {
    bool test;
    std::string SH_COM_PORT;
    SIMHUB_CONF() : test(false), SH_COM_PORT("COM1") {}
} SIMHUB_CONF; SIMHUB_CONF SIMHUB_Conf;

typedef struct VOLANTE_CONF {
    const uint8_t BOARD_REV;
    bool MULTI_BOARD;

    VOLANTE_CONF() : BOARD_REV(1), MULTI_BOARD(true) {}
} VOLANTE_CONF; VOLANTE_CONF VOLANTE_Conf;

typedef struct PEDALS_CONF {
    bool RAW_ATTACHED;
    bool SIMULATE_ATTACHED; //BY ACQUIRING THE XInput or DirectInput example: https://github.com/walbourn/directx-sdk-samples/tree/master/DirectInput
    
    PEDALS_CONF() : RAW_ATTACHED(false), SIMULATE_ATTACHED(false) {}
} PEDALS_CONF; PEDALS_CONF PEDALS_Conf;

