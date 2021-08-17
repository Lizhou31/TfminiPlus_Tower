/***********************************************************************
* FILENAME :        i2c.h
*
* DESCRIPTION :
*       Reference from STM32 HAL Lib
*
* COPYRIGHT :
*       Copyright Lizhou 2021 All rights reserved.
*
* ORIGINAL AUTHOR(s) :    lisie31s@gmail.com        
*
* CREATE DATE :    10 March 2021
*
* CHANGES :
*       DATE        WHO           DETAIL
*       19MAR21     Lizhou        First Release Version.
*       17AUG21     Lizhou        Optimize and fix some Error
*
*************************************************************************/

#ifndef __I2C_H
#define __I2C_H

#include "stm32f1xx_ll_i2c.h"
#include "PV_SystemTick.h"

#define I2C_MAX_DELAY 0xFFFFU

typedef enum
{
    I2C_ERROR_NONE = 0x000U,
    I2C_ERROR_BERR = 0x001U,
    I2C_ERROR_ARLO = 0x002U,
    I2C_ERROR_AF = 0x004U,
    I2C_ERROR_OVR = 0x008U,
    I2C_ERROR_TIMEOUT = 0x020U,
    I2C_ERROR_SIZE = 0x040U,
    I2C_WRONG_START = 0x200U,
    I2C_ERROR_DISABLE = 0x400U
} I2C_Error_Code;

/** 
  * @brief  i2cHandler_struct
  */
typedef struct
{
    I2C_TypeDef *Instance;
    __IO I2C_Error_Code ErrorCode;
    __IO uint16_t XferCount;
} I2CHandler;

void i2chandler_init(I2CHandler *i2chandler, I2C_TypeDef *Instance);
uint8_t I2C_Master_Transmit(I2CHandler *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t size, uint32_t Timeout);
uint8_t I2C_Master_Receive(I2CHandler *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t size, uint32_t Timeout);
#endif /* __I2C_H */