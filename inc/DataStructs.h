#pragma once

#include <iostream>

typedef struct LCD_DATA_V2 {
  uint8_t gear__rpm_leds; // 4 + 4
  uint16_t tyre_temp_front; // 8 + 8
  uint16_t tyre_temp_rear; // 8 + 8
  uint16_t tyre_wear_front__tc_active__abs_active; // 7 + 7 + 1 + 1
  uint16_t tyre_wear_rear__2NULL; // 7 + 7 + 2NULL
  uint16_t current_lap__total_laps; // 8 + 8
  uint16_t speed__position; // 10 + 6
  uint16_t brake_temp_RR__tc_level__flags; // 10 + 3 + 3
  uint32_t best_lap__fuel_perc__DRS_ACTIVE__DRS__ENABLED; // (7 + 6 + 10) + 7 + 1 + 1
  uint64_t delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level; // (7 + 6 + 10) + (7 + 6 + 10) + 7 + 7 + 1 + 3
  uint32_t brake_temp_FL_FR_RL__game_running__NULL; // 10 + 10 + 10 + 1 + 1 
  // Total of 31 bytes 
} LCD_DATA_V2;

typedef struct LCD_DATA {
    uint8_t gear; //4 bit 
    uint16_t speed; //10 bit
    uint8_t position; //6 bit 
    uint8_t currentLap; // 8 bit
    uint8_t totalLaps; // 8 bit
    char bestLap[16]; // MM : SS: DCM  // 7bit : 6 bit : 10 bit
    char deltaToBestLap[16]; // MM : SS: DCM  // 7bit : 6 bit : 10 bit
    char allTimeBestDelta[16]; // MM : SS: DCM  // 7bit : 6 bit : 10 bit
    uint8_t DRS_Available; // 1 bit
    uint8_t DRS_Enabled; // 1 bit
    uint8_t fuel_percentage; // 7 bit
    uint16_t tyre_tempFL; // 8bit
    uint16_t tyre_tempFR; // 8bit
    uint16_t tyre_tempRL; // 8bit
    uint16_t tyre_tempRR; // 8bit
    uint8_t ERS_percentage; // 7bit
    uint8_t brake_bias; //7 bit 
    uint8_t rpm_leds_on; // 4 bit
    char flags[16]; //3 bit = 8 different values
    uint8_t game_running; // 1bit
    uint8_t abs_level; // 3bit
    uint8_t abs_active; // 2bit = off, low, medium, full
    uint16_t brake_tempFL; // 10 bit
    uint16_t brake_tempFR; // 10 bit
    uint16_t brake_tempRL; // 10 bit
    uint16_t brake_tempRR; // 10 bit
    uint8_t isInPitLane; // 1bit
    uint8_t tc_level; // 3bit
    uint8_t tc_active; // 2 bit
    uint16_t tyre_wearFL; // 7bit
    uint16_t tyre_wearFR; // 7bit
    uint16_t tyre_wearRL; // 7bit
    uint16_t tyre_wearRR; // 7bit 
    //HERE WE HAVE 106 BYTES
} LCD_DATA;


typedef struct VolanteData {
  uint16_t mcp_btn;
  uint16_t mcp_enc;
  uint16_t mcp_ext;
  uint16_t rw1;
  uint16_t rw2;
  uint16_t rw3;
  uint16_t rw4;
  uint16_t rw5;
  uint16_t rw6;
  uint16_t clutch;
  uint16_t tg0;
} VolanteData;