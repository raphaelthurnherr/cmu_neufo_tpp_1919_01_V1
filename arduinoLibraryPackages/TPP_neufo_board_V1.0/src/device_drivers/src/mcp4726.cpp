/**
 * \file mcp4726.h
 * \brief MCP4726 Digital analog converter with eeprom memory driver
 *  I2C default address: 0x60 when ADR connected to GND
 * \author Raphael Thurnherr
 * \version 0.1
 * \date 24.04.2019
 *
 * Library to setup and drive the DAC converter MCP4726
 * 
 */

#ifndef I2CSIMU

// user Definitions
#define RESOLUTION  4095


#include <Arduino.h>
#include "mcp4726.h"
#include "arduino-i2c.h"

#include "stdio.h"
#include "math.h"

/**
 * \brief MCP4726 driver initialization
 * \param pointer on the configuration structure
 * \return code error
 */
int mcp4726_init(device_mcp4726 *mcp4726config){
    int err =0;    
    unsigned char mcp4726_regData[32];
    unsigned char deviceAddress = mcp4726config->deviceAddress;

    // Get the DAC register setting (8 bit), DAC Register data (16 bit), EEPROM DATA(16 bit)   
    err += i2c_readRaw(0, deviceAddress, mcp4726_regData , 5);

    if(err){
        printf("Kehops I2C MCP4726 device initialization with %d error\n", err);
    }
    return err;
}


/**
 * \brief MCP4726 set DAC output voltage in mV
 * \param pointer on the configuration structure
 * \param value_mv, mV value for output
 * \return code error
 */

int mcp4726_setDACOutput_mV(device_mcp4726 *mcp4726config, int value_mv){
    unsigned char err =0;
    unsigned char deviceAddress = mcp4726config->deviceAddress;
    unsigned int vref = mcp4726config->vref_mv;
    unsigned int regData =0;
    float resolution_mv;
    unsigned char data[32];
    
    resolution_mv = (float)vref / RESOLUTION;
    
    // Ensure that the register value not exced the DAC IC 12bit value
    if(value_mv<0) value_mv =0;
    else if (value_mv>5000) value_mv= 5000;

    regData = round((float)value_mv / resolution_mv);
  
  
    
    // Write only DAC Register: (C2, C1, C0) = (0,1,0) and  POWERDOWN =0;
    data[0] = 0x40;
    
    // Loading buffer with 12bit register value
    data[1] = (regData & 0x0ff0) >> 4;        // MSB
    data[2] = (regData & 0x000f) << 4;        // LSB
    
    err += i2c_writeRaw(0, deviceAddress, data, 3);
    
    printf("Write I2C mcp4726_setDACOutput_mV:     valueMV: %d     Address:   0x%2x", value_mv,  deviceAddress );
    err++;
    return err;
}

/**
 * \brief MCP4726 set DAC output 12bit value
 * \param pointer on the configuration structure
 * \param value (12bit), mV value for output
 * \return code error
 */

int mcp4726_setDAC_12bitValue(device_mcp4726 *mcp4726config, int value){
    unsigned char err =0;
    unsigned char deviceAddress = mcp4726config->deviceAddress;
    unsigned char data[32];
    
    // Write only DAC Register: (C2, C1, C0) = (0,1,0) and  POWERDOWN =0;
    data[0] = 0x40;
    

    // Ensure that the register value not exced the DAC IC 12bit value
    if(value<0) value =0;
    else if (value>4095) value= 4096;

    // Loading buffer with 12bit register value
    data[1] = (value & 0x0ff0) >> 4;        // MSB
    data[2] = (value & 0x000f) << 4;        // LSB

    err += i2c_writeRaw(0, deviceAddress, data, 3);
    
    err++;
    return err;
}

#endif