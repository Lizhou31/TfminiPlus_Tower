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
*       17AUG21     Lizhou        Optimize and fix some Error
*
*************************************************************************/

#include "tfmini_plus.h"

uint8_t tfmini_channel_count;

static uint8_t checkSum(uint8_t *data, uint16_t Size)
{
    uint16_t temp = 0;
    for (uint16_t i = 0; i < (Size - 1); i++)
    {
        temp += (uint16_t)data[i];
    }
    return (uint8_t)temp;
}

static uint8_t getData(tfmini_handler *tfmini, uint8_t *pData)
{
    uint8_t cmd[5];
    cmd[0] = 0x5A;
    cmd[1] = 0x05;
    cmd[2] = 0x00;
    cmd[3] = 0x01;
    cmd[4] = checkSum(cmd, 5);
    if (I2C_Master_Transmit(&i2chandler, (tfmini->DevAddress) << 1, cmd, sizeof(cmd), 0xf))
    {
        tfmini->ErrorCode = TFMINI_GETDATA_ERROR_WRITE;
        return 1;
    }
    if (I2C_Master_Receive(&i2chandler, (tfmini->DevAddress) << 1, pData, 9U, 0xf))
    {
        tfmini->ErrorCode = TFMINI_GETDATA_ERROR_READ;
        return 1;
    }
    return 0;
}

static uint8_t getData_mutiplexer(tfmini_handler *tfmini, uint8_t channel_mask)
{
    uint8_t channel[1] = {0b00000000};
    channel[0] |= channel_mask;
    if (I2C_Master_Transmit(&i2chandler, (0x70) << 1, channel, 1U, 0xf))
    {
        tfmini->ErrorCode = TFMINI_MUL_ERROROPEN;
        return 1;
    }

    if (getData(tfmini, tfmini->rawdata))
    {
        return 1;
    }

    channel[0] &= 0b00000000;
    if (I2C_Master_Transmit(&i2chandler, (0x70) << 1, channel, 1U, 0xf))
    {
        tfmini->ErrorCode = TFMINI_MUL_ERRORCLOSE;
        return 1;
    }
    return 0;
}

uint8_t ScanDistanceSensor(tfmini_handler *tfmini)
{
    uint8_t cmd[1];
    for (uint8_t channel_mask = 0; channel_mask < 5; channel_mask++)
    {
        uint8_t channel[1] = {0b00000001 << channel_mask};
        if (I2C_Master_Transmit(&i2chandler, (0x70) << 1, (channel), 1U, 0xf))
        {
            return 1;
        }
        if (!I2C_Master_Transmit(&i2chandler, (tfmini->DevAddress) << 1, cmd, 0U, 0xf))
        {
            tfmini_channel_count |= channel[0];
        }
        channel[0] &= 0b00000000;
        if (I2C_Master_Transmit(&i2chandler, (0x70) << 1, channel, 1U, 0xf))
        {
            return 1;
        }
        tfmini = tfmini + 1;
    }
    return 0;
}

uint8_t updateDistance(tfmini_handler *tfmini)
{
    uint8_t cmd[5];
    cmd[0] = 0x5A;
    cmd[1] = 0x05;
    cmd[2] = 0x00;
    cmd[3] = 0x01;
    cmd[4] = checkSum(cmd, 5);
    if (getData_mutiplexer(tfmini, tfmini->channel))
    {
        return 1;
    }
    if (tfmini->rawdata[0] != 0x59 || tfmini->rawdata[1] != 0x59)
    {
        tfmini->ErrorCode = TFMINI_DATAHEADER_FAILED;
        return 1;
    }
    if (checkSum(tfmini->rawdata, 0x09) != tfmini->rawdata[8])
    {
        tfmini->ErrorCode = TFMINI_CHECKSUM_FAILED;
        return 1;
    }
    tfmini->Distance = ((uint16_t)tfmini->rawdata[3] << 8 | (uint16_t)tfmini->rawdata[2]);
    return 0;
}