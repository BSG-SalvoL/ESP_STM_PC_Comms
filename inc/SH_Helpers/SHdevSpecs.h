#ifndef __SHDEVSPECS__
#define __SHDEVSPECS__

#include <iostream>
#include <string>

using namespace std;

const string COM_NAME = "SimHub";
const string DEVICE_NAME = "SalvoG Steering Wheel";
#define Rev  'j'
#define Signature1  0x1E
#define Signature2  0x98
#define Signature3  0x01
#define Header  0x03
#define ENABLE_MICRO_GAMEPAD  0
#define MICRO_GAMEPAD_ENCODERPRESSTIME  50
#define ENABLED_BUTTONS_COUNT  0

// ----------------------------------   OTHER UNUSED BUT NECESSARY DEFINES   -----------------------------

#define ENABLED_BUTTONMATRIX  0
#define BMATRIX_COLS  0
#define BMATRIX_ROWS  0
#define TM1638_ENABLEDMODULES  0
#define MAX7221_ENABLEDMODULES  0
#define TM1637_ENABLEDMODULES  0
#define ENABLE_ADA_HT16K33_7SEGMENTS  0
#define ENABLED_ENCODERS_COUNT  0
#define MAX7221_MATRIX_ENABLED  0
#define ENABLE_ADA_HT16K33_BiColorMatrix  0
#define ENABLE_ADA_HT16K33_SingleColorMatrix  0
#define I2CLCD_enabled  0
#define ENABLED_NOKIALCD  0
#define ENABLED_OLEDLCD  0
#define WS2812B_MATRIX_ENABLED  0
#define WS2812B_RGBLEDCOUNT  0
#define PL9823_RGBLEDCOUNT  0
#define WS2801_RGBLEDCOUNT  0
#define J_Command  (ENABLED_BUTTONS_COUNT + ENABLED_BUTTONMATRIX * (BMATRIX_COLS * BMATRIX_ROWS))
#define B_Command  (MAX7221_ENABLEDMODULES + TM1637_ENABLEDMODULES + ENABLE_ADA_HT16K33_7SEGMENTS)
#define _4_Command  (WS2812B_RGBLEDCOUNT + PL9823_RGBLEDCOUNT + WS2801_RGBLEDCOUNT)

#endif