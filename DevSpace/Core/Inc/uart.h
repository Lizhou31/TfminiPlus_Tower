/***********************************************************************
* FILENAME :        uart.h
*
* DESCRIPTION :
*       Reference from STM32 HAL Lib
*
* COPYRIGHT :
*       Copyright Lizhou 2021 All rights reserved.
*
* ORIGINAL AUTHOR(s) :    lisie31s@gmail.com        
*
* CREATE DATE :    19 March 2021
*
* CHANGES :
*       DATE        WHO           DETAIL
*
*************************************************************************/

#ifndef __UART_H
#define __UART_H

#include "PV_SystemTick.h"
#include "stm32f1xx_ll_usart.h"
#define MAX_DELAY 0xFFFFFFFFU

/** @defgroup UART_Error_Code UART Error Code
  * @{
  */
#define HAL_UART_ERROR_NONE 0x00000000U /*!< No error            */
#define HAL_UART_ERROR_PE 0x00000001U   /*!< Parity error        */
#define HAL_UART_ERROR_NE 0x00000002U   /*!< Noise error         */
#define HAL_UART_ERROR_FE 0x00000004U   /*!< Frame error         */
#define HAL_UART_ERROR_ORE 0x00000008U  /*!< Overrun error       */
#define HAL_UART_ERROR_DMA 0x00000010U  /*!< DMA transfer error  */
#define HAL_UART_ERROR_TIMEOUT 0x00000020U
/**
  * @}
  */

typedef struct
{
    USART_TypeDef *Instance;
    uint8_t *pTxBuffptr;
    uint16_t TxXferSize;
    __IO uint16_t TxXferCount;
    __IO uint32_t ErrorCode;
} uartHandler, UARTHANDLER[1];

int8_t UART_Transmit(uartHandler *huart, uint8_t *pData, uint16_t Size, uint32_t timeout);
#endif /* __UART_H */