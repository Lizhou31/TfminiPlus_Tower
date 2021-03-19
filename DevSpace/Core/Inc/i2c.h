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
*
*************************************************************************/

#ifndef __I2C_H
#define __I2C_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_i2c.h"
#include "PV_SystemTick.h"

#define MAX_DELAY 0xFFFFFFFFU

/** @defgroup I2C_Error_Code_definition I2C Error Code definition
  * @brief  I2C Error Code definition
  * @{
  */
#define HAL_I2C_ERROR_NONE 0x00000000U      /*!< No error              */
#define HAL_I2C_ERROR_BERR 0x00000001U      /*!< BERR error            */
#define HAL_I2C_ERROR_ARLO 0x00000002U      /*!< ARLO error            */
#define HAL_I2C_ERROR_AF 0x00000004U        /*!< AF error              */
#define HAL_I2C_ERROR_OVR 0x00000008U       /*!< OVR error             */
#define HAL_I2C_ERROR_DMA 0x00000010U       /*!< DMA transfer error    */
#define HAL_I2C_ERROR_TIMEOUT 0x00000020U   /*!< Timeout Error         */
#define HAL_I2C_ERROR_SIZE 0x00000040U      /*!< Size Management error */
#define HAL_I2C_ERROR_DMA_PARAM 0x00000080U /*!< DMA Parameter Error   */
#define HAL_I2C_WRONG_START 0x00000200U     /*!< Wrong start Error     */
#define HAL_I2C_ERROR_DISABLE 0x00000400U   /*!< I2C Disable Error     */
/**
  * @}
  */

/** @defgroup HAL_state_structure_definition HAL state structure definition
  * @brief  HAL State structure definition
  * @note  HAL I2C State value coding follow below described bitmap :
  *          b7-b6  Error information
  *             00 : No Error
  *             01 : Abort (Abort user request on going)
  *             10 : Timeout
  *             11 : Error
  *          b5     Peripheral initilisation status
  *             0  : Reset (Peripheral not initialized)
  *             1  : Init done (Peripheral initialized and ready to use. HAL I2C Init function called)
  *          b4     (not used)
  *             x  : Should be set to 0
  *          b3
  *             0  : Ready or Busy (No Listen mode ongoing)
  *             1  : Listen (Peripheral in Address Listen Mode)
  *          b2     Intrinsic process state
  *             0  : Ready
  *             1  : Busy (Peripheral busy with some configuration or internal operations)
  *          b1     Rx state
  *             0  : Ready (no Rx operation ongoing)
  *             1  : Busy (Rx operation ongoing)
  *          b0     Tx state
  *             0  : Ready (no Tx operation ongoing)
  *             1  : Busy (Tx operation ongoing)
  * @{
  */
typedef enum
{
  HAL_I2C_STATE_RESET             = 0x00U,   /*!< Peripheral is not yet Initialized         */
  HAL_I2C_STATE_READY             = 0x20U,   /*!< Peripheral Initialized and ready for use  */
  HAL_I2C_STATE_BUSY              = 0x24U,   /*!< An internal process is ongoing            */
  HAL_I2C_STATE_BUSY_TX           = 0x21U,   /*!< Data Transmission process is ongoing      */
  HAL_I2C_STATE_BUSY_RX           = 0x22U,   /*!< Data Reception process is ongoing         */
  HAL_I2C_STATE_LISTEN            = 0x28U,   /*!< Address Listen Mode is ongoing            */
  HAL_I2C_STATE_BUSY_TX_LISTEN    = 0x29U,   /*!< Address Listen Mode and Data Transmission
                                                 process is ongoing                         */
  HAL_I2C_STATE_BUSY_RX_LISTEN    = 0x2AU,   /*!< Address Listen Mode and Data Reception
                                                 process is ongoing                         */
  HAL_I2C_STATE_ABORT             = 0x60U,   /*!< Abort user request ongoing                */
  HAL_I2C_STATE_TIMEOUT           = 0xA0U,   /*!< Timeout state                             */
  HAL_I2C_STATE_ERROR             = 0xE0U    /*!< Error                                     */

} HAL_I2C_StateTypeDef;
/**
  * @}
  */

/** 
  * @brief  i2cHandler_struct
  */
typedef struct
{
    I2C_TypeDef                   *Instance;
    uint8_t                       *pBuffPtr;
    uint16_t                      XferSize;
    __IO uint16_t                 XferCount;
    __IO HAL_I2C_StateTypeDef     State;
    __IO uint32_t                 ErrorCode;
} i2cHandler, I2CHANDLER[1];

int8_t I2C_Master_Transmit(i2cHandler *hi2c, uint8_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
int8_t I2C_Master_Receive(i2cHandler *hi2c, uint8_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
void i2c_handler_Init(i2cHandler *i2c, I2C_TypeDef *I2C);
#endif /* __I2C_H */