/***********************************************************************
* FILENAME :        uart.c
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
#include "uart.h"

/**
 * @brief   Static function to Wait UART TXE Flag Set until timeout
 * @param   huart: uartHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t UART_WaitOnTXEFlagUntilTimeout(uartHandler *huart, uint32_t tickstart, uint32_t Timeout)
{
    while (!LL_USART_IsActiveFlag_TXE(huart->Instance))
    {
        if (Timeout != MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
                CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE));
                CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

                huart->ErrorCode = HAL_UART_ERROR_TIMEOUT;
                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   Static function to Wait UART TC Flag Set until timeout
 * @param   huart: uartHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t UART_WaitOnTCFlagUntilTimeout(uartHandler *huart, uint32_t tickstart, uint32_t Timeout)
{
    while (!LL_USART_IsActiveFlag_TC(huart->Instance))
    {
        if (Timeout != MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
                CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE));
                CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

                huart->ErrorCode = HAL_UART_ERROR_TIMEOUT;
                return -1;
            }
        }
    }
    return 0;
}

int8_t UART_Transmit(uartHandler *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    uint32_t tickstart = GetTick();
    huart->ErrorCode = HAL_UART_ERROR_NONE;

    huart->pTxBuffptr = pData;
    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    while (huart->TxXferCount > 0U)
    {
        if (UART_WaitOnTXEFlagUntilTimeout(huart, tickstart, Timeout))
            return -1;
        LL_USART_TransmitData8(huart->Instance, (uint8_t)(*(huart->pTxBuffptr) & 0xFFU));
        huart->pTxBuffptr++;
        huart->TxXferCount--;
    }
    if (UART_WaitOnTXEFlagUntilTimeout(huart, tickstart, Timeout))
        return -1;
    if (UART_WaitOnTCFlagUntilTimeout(huart, tickstart, Timeout))
        return -1;

    return 0;
}