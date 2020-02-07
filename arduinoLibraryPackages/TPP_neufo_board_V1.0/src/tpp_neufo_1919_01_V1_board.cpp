#include <Arduino.h>
#include <Wire.h>
#include "tpp_neufo_1919_01_V1_board.h"

board_1919_01_V01::board_1919_01_V01(void){
    Wire.begin();                       // Initiate the Wire library for I2C
    
    CHANNEL_A_IC2_DAC.deviceAddress = 0x60;
    CHANNEL_A_IC2_DAC.vref_mv = 5000;

    CHANNEL_B_IC3_DAC.deviceAddress = 0x63;
    CHANNEL_B_IC3_DAC.vref_mv = 5000;
}

void board_1919_01_V01::begin(void){

    // Setting up the mcp23017 GPIO extender
    Serial.print("CONFIG ARDUINO GPIO MODE... ");

    
    pinMode(6, INPUT);           // set TRIG pin to input        
    pinMode(7, INPUT);           // set BUTTON pin to input  
    pinMode(8, INPUT);           // set LICKPORT pin to input
 
    pinMode(9, OUTPUT);          // set RGB RED LED pin to output  
    pinMode(10, OUTPUT);         // set RGB GREEN LED pin to output  
    pinMode(11, OUTPUT);         // set RGB BLUE LED pin to output  

    pinMode(6, INPUT);           // set TRIG pin to input

    pinMode(A0, INPUT);          // set analog pin A0 as input
    //pinMode(A7, INPUT);          // set analog pin A7 as input


    Serial.println("DONE ");

    Serial.print("CONFIG I2C DAC IC... ");
    mcp4726_init(&CHANNEL_A_IC2_DAC);
    mcp4726_init(&CHANNEL_B_IC3_DAC);

    Serial.println("DONE ");
}

int  board_1919_01_V01::setAnalogOutput_mV(unsigned char channel, int value){
    int result = -1;

        switch(channel){
            case CHANNEL_1 : mcp4726_setDACOutput_mV(&CHANNEL_A_IC2_DAC, value); break;
            case CHANNEL_2 : mcp4726_setDACOutput_mV(&CHANNEL_B_IC3_DAC, value); break;
            default : break;
        }
        

    return result;
}

int  board_1919_01_V01::setAnalogOutput_value(unsigned char channel, int value){
    int result = -1;

        switch(channel){
            case CHANNEL_1 : mcp4726_setDAC_12bitValue(&CHANNEL_A_IC2_DAC, value); break;
            case CHANNEL_2 : mcp4726_setDAC_12bitValue(&CHANNEL_B_IC3_DAC, value); break;
            default : break;
        }
        

    return result;
}