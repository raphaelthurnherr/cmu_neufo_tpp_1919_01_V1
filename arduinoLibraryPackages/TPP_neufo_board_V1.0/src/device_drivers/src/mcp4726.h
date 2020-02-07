/**
 * \file mcp4726.h
 * \brief MCP4726 Digital analog converter with eeprom memory driver
 *  I2C default address: 0x60 (7 bit)
 * \author Raphael Thurnherr
 * \version 0.1
 * \date 24.04.2019
 *
 * Library to setup and drive the DAC converter MCP4726
 * 
 */

#ifndef MCP4726_H
#define MCP4726_H

/**
 * \struct mcp4726 [mcp4726.h] Configuration structure definition
 */

typedef struct mcp4726{
    char deviceName[25];                        // Device Name of IC
    unsigned char deviceAddress;                // Bus device address
    unsigned int vref_mv;                       // VoltageReference value in mV (2.7..5.5)
} device_mcp4726;

/**
 * \brief MCP4726 driver initialization
 * \param pointer on the configuration structure
 * \return code error
 */
extern int mcp4726_init(device_mcp4726 *mcp4726config);        // MCP4726 driver initialization

/**
 * \brief MCP4726 set output voltage in mV
 * \param pointer on the configuration structure
 * \return code error
 */
extern int mcp4726_setDACOutput_mV(device_mcp4726 *mcp4726config, int value_mv);

/**
 * \brief MCP4726 set output voltage by 12 bit value
 * \param pointer on the configuration structure
 * \return code error
 */
extern int mcp4726_setDAC_12bitValue(device_mcp4726 *mcp4726config, int value);

#endif /* MCP4726_H */

