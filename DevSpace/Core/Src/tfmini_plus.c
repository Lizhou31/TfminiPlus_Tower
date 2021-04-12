/***********************************************************************
* FILENAME :        tfmini_plus.c
*
* DESCRIPTION :
*       tfmini-plus lib for stm32 
*
* COPYRIGHT :
*       Copyright Lizhou 2021 All rights reserved.
*
* ORIGINAL AUTHOR(s) :    lisie31s@gmail.com        
*
* CREATE DATE :    17 March 2021
*
* CHANGES :
*       DATE        WHO           DETAIL
*       19MAR21     Lizhou        First Release Version.
*
*************************************************************************/

#include "tfmini_plus.h"

static uint8_t checkSum(uint8_t *data, uint16_t Size)
{
    uint16_t temp = 0;
    for (uint16_t i = 0; i < (Size - 1); i++)
    {
        temp += (uint16_t)data[i];
    }
    return (uint8_t)temp;
}

static int8_t getData_cmd(tfmini_handler *tfmini, uint16_t transSize, uint16_t receiSize, uint8_t channel)
{
    uint8_t channelmask[1] = {0b00000000};
    channelmask[0] |= channel;
    if (I2C_Master_Transmit(tfmini->hi2c, (0x70) << 1, channelmask, 1U, 0xf))
    {
        return -1;
    }

    if (I2C_Master_Transmit(tfmini->hi2c, (tfmini->DevAddress) << 1, tfmini->cmd, transSize, 0xf))
    {
        tfmini->ErrorCode = GETDATA_ERROR_WRITE;
        return -1;
    }
    if (I2C_Master_Receive(tfmini->hi2c, (tfmini->DevAddress) << 1, tfmini->data, receiSize, 0xf))
    {
        tfmini->ErrorCode = GETDATA_ERROR_READ;
        return -1;
    }

    channelmask[0] &= 0b00000000;
    if (I2C_Master_Transmit(tfmini->hi2c, (0x70) << 1, channelmask, 1U, 0xf))
    {
        return -1;
    }
    return 0;
}

int8_t fetchDistance(tfmini_handler *tfmini)
{

    tfmini->cmd[0] = 0x5A;
    tfmini->cmd[1] = 0x05;
    tfmini->cmd[2] = 0x00;
    tfmini->cmd[3] = 0x01;
    tfmini->cmd[4] = checkSum(tfmini->cmd, 5);
    if (getData_cmd(tfmini, 0x05, 0x09, tfmini->channel))
        return -1;
    if (tfmini->data[0] != 0x59 || tfmini->data[1] != 0x59)
    {
        tfmini->ErrorCode = DATAHEADER_FAILED;
        return -1;
    }
    if (checkSum(tfmini->data, 0x09) != tfmini->data[8])
    {
        tfmini->ErrorCode = CHECKSUM_FAILED;
        return -1;
    }
    tfmini->Distance = ((uint16_t)tfmini->data[3] << 8 | (uint16_t)tfmini->data[2]);
    return 0;
}