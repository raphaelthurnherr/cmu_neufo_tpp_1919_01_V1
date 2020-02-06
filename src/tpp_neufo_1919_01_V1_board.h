#ifndef tpp_neufo_1919_01_V1_board_h
#define tpp_neufo_1919_01_V1_board_h

#include <Arduino.h>
//Low level device hardware library
#include "device_drivers/src/mcp4726.h"


#define CN3_TRIG_PIN 6
#define SW1_BUTTTON_PIN 7
#define CN2_LICKPORT_PIN 8

#define RGB_LED_RED  9
#define RGB_LED_GREEN 10
#define RGB_LED_BLUE  11

#define CHANNEL_1 0
#define CHANNEL_2 1
#define CN7 0
#define CN8 1

#define RV1_POT_PIN A7
#define BT1_BATTERY_PIN A0

class board_1919_01_V01{
    public:

    board_1919_01_V01(void);
    void begin(void);
    int setAnalogOutput_mV(unsigned char channel, int value);
    int setAnalogOutput_value(unsigned char channel, int value);

    protected:
        device_mcp4726 CHANNEL_A_IC2_DAC;
        device_mcp4726 CHANNEL_B_IC3_DAC;
};

#endif