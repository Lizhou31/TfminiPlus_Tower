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
*
*************************************************************************/
#ifndef __TFMINI_PLUS_H
#define __TFMINI_PLUS_H

#include "i2c.h"
#define GETDATA_ERROR_WRITE 0x01
#define GETDATA_ERROR_READ 0x02
#define DATAHEADER_FAILED 0x04
#define CHECKSUM_FAILED 0x08

typedef struct
{
    i2cHandler *hi2c;
    uint8_t DevAddress;
    uint8_t cmd[5];
    uint8_t data[10];
    uint8_t channel;
    uint16_t Distance;
    uint8_t ErrorCode;
} tfmini_handler, TFMINI_HANDLER[0];

int8_t fetchDistance(tfmini_handler *tfmini);
int8_t ScanDistanceSensor(tfmini_handler *tfmini);
extern uint8_t tfmini_channel_count;
#endif /* __TFMINI_PLUS_H */