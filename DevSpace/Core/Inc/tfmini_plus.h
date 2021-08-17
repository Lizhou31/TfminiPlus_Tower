/***********************************************************************
* FILENAME :        tfmini_plus.h
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
#ifndef __TFMINI_PLUS_H
#define __TFMINI_PLUS_H

#include "main.h"

extern I2CHandler i2chandler;

typedef enum
{
    TFMINI_ERROR_NONE = 0x00U,
    TFMINI_GETDATA_ERROR_WRITE = 0x01U,
    TFMINI_GETDATA_ERROR_READ = 0x02U,
    TFMINI_DATAHEADER_FAILED = 0x03U,
    TFMINI_CHECKSUM_FAILED = 0x04U,
    TFMINI_MUL_ERROROPEN = 0x05U,
    TFMINI_MUL_ERRORCLOSE = 0x06U
} TFMINI_Plus_Error;

typedef struct
{
    uint8_t DevAddress;
    uint8_t channel;
    uint8_t rawdata[9];
    uint16_t Distance;
    TFMINI_Plus_Error ErrorCode;
} tfmini_handler, TFMINI_HANDLER[0];

uint8_t updateDistance(tfmini_handler *tfmini);
uint8_t ScanDistanceSensor(tfmini_handler *tfmini);

extern uint8_t tfmini_channel_count;
#endif /* __TFMINI_PLUS_H */