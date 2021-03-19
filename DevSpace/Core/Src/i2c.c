/***********************************************************************
* FILENAME :        i2c.c
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
*       10MAR21     Lizhou        First Release Version.
*
*************************************************************************/
#include "i2c.h"

/**
  * @brief  Initialize i2c periphery.(SPI, GPIO)
  * @param  i2c: i2chandler struct (with some state define)
  * @param  I2C: SPL Default I2C struct (with registers)
  * @param  Mode: I2C Operation Mode (That is no effect now)
  * @retval None
  */
void i2c_handler_Init(i2cHandler *i2c, I2C_TypeDef *I2C)
{
    i2c->Instance = I2C;
    i2c->State = HAL_I2C_STATE_READY;
    i2c->ErrorCode = HAL_I2C_ERROR_NONE;
}

/**
 * @brief   Static function to Wait Busy Flag Reset until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnBUSYFlagUntilTimeout(i2cHandler *hi2c, uint32_t tickstart, uint32_t Timeout)
{
    while (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance))
    {
        /* Check for the Timeout */
        if (Timeout < MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   Static function to Wait SB Flag SET until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnSBFlagUntilTimeout(i2cHandler *hi2c, uint32_t tickstart, uint32_t Timeout)
{
    while (!LL_I2C_IsActiveFlag_SB(hi2c->Instance))
    {
        /* Check for the Timeout */
        if (Timeout < MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
                if (READ_BIT(hi2c->Instance->CR1, I2C_CR1_START) == I2C_CR1_START)
                    hi2c->ErrorCode = HAL_I2C_WRONG_START;
                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   Static function to Wait ADDR Flag SET until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnADDRFlagUntilTimeout(i2cHandler *hi2c, uint32_t tickstart, uint32_t Timeout)
{
    while (!LL_I2C_IsActiveFlag_ADDR(hi2c->Instance))
    {
        /* Check if a NACK is detected */
        if (LL_I2C_IsActiveFlag_AF(hi2c->Instance))
        {
            /* Generate Stop */
            SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

            /* Clear AF Flag */
            CLEAR_BIT(hi2c->Instance->SR1, I2C_SR1_AF);

            hi2c->State = HAL_I2C_STATE_READY;
            hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
            return -1;
        }

        /* Check for the Timeout */
        if (Timeout < MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   Static function to Wait TXE Flag SET until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnTXEFlagUntilTimeout(i2cHandler *hi2c, uint32_t tickstart, uint32_t Timeout)
{
    while (!LL_I2C_IsActiveFlag_TXE(hi2c->Instance))
    {
        /* Check if a NACK is detected */
        if (LL_I2C_IsActiveFlag_AF(hi2c->Instance))
        {
            /* Generate Stop */
            SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

            /* Clear AF Flag */
            CLEAR_BIT(hi2c->Instance->SR1, I2C_SR1_AF);

            hi2c->State = HAL_I2C_STATE_READY;
            hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
            return -1;
        }

        /* Check for the Timeout */
        if (Timeout < MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   Static function to Wait BTF Flag SET until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnBTFFlagUntilTimeout(i2cHandler *hi2c, uint64_t tickstart, uint64_t Timeout)
{
    while (!LL_I2C_IsActiveFlag_BTF(hi2c->Instance))
    {
        /* Check if a NACK is detected */
        if (LL_I2C_IsActiveFlag_AF(hi2c->Instance))
        {
            /* Generate Stop */
            SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

            /* Clear AF Flag */
            CLEAR_BIT(hi2c->Instance->SR1, I2C_SR1_AF);

            hi2c->State = HAL_I2C_STATE_READY;
            hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
            return -1;
        }
        /* Check for the Timeout */
        if (Timeout < MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   Static function to Wait RXNE Flag SET until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnRXNEFlagUntilTimeout(i2cHandler *hi2c, uint64_t tickstart, uint64_t Timeout)
{
    while (LL_I2C_IsActiveFlag_RXNE(hi2c->Instance))
    {
        if (LL_I2C_IsActiveFlag_STOP(hi2c->Instance))
        {
            /* Clear STOP Flag */
            LL_I2C_ClearFlag_STOP(hi2c->Instance);

            hi2c->State = HAL_I2C_STATE_READY;
            hi2c->ErrorCode |= HAL_I2C_ERROR_NONE;
            return -1;
        }
        /* Check for the Timeout */
        if (Timeout < MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
                return -1;
            }
        }
    }
    return 0;
}

int8_t I2C_Master_Transmit(i2cHandler *hi2c, uint8_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /* Init tickstart for timeout management*/
    uint32_t tickstart = GetTick();

    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        /* Wait until BUSY flag is reset */
        if (I2C_WaitOnBUSYFlagUntilTimeout(hi2c, tickstart, Timeout))
            return -1;
        
        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
        {
            hi2c->State = HAL_I2C_STATE_READY;
            hi2c->ErrorCode |= HAL_I2C_ERROR_DISABLE;
            return -1;
        }

        /* Disable Pos */
        CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

        hi2c->State = HAL_I2C_STATE_BUSY_TX;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
        hi2c->pBuffPtr = pData;
        hi2c->XferCount = Size;
        hi2c->XferSize = hi2c->XferCount;

        /* Generate Start */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);

        /* Wait until SB flag is set */
        if (I2C_WaitOnSBFlagUntilTimeout(hi2c, tickstart, Timeout))
            return -1;

        /* Send slave address (W)*/
        LL_I2C_TransmitData8(hi2c->Instance, ((uint8_t)((DevAddress) & (uint8_t)(~I2C_OAR1_ADD0))));

        /* Wait until ADDR flag is set */
        if (I2C_WaitOnADDRFlagUntilTimeout(hi2c, tickstart, Timeout))
            return -1;

        /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(hi2c->Instance);

        /* Wait until TXE flag is set */
        if (I2C_WaitOnTXEFlagUntilTimeout(hi2c, tickstart, Timeout))
            return -1;

        while (hi2c->XferCount > 0U)
        {
            LL_I2C_TransmitData8(hi2c->Instance, *hi2c->pBuffPtr);

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;

            /* Update counter */
            hi2c->XferCount--;

            if (I2C_WaitOnBTFFlagUntilTimeout(hi2c, tickstart, Timeout))
                return -1;
        }
        /* Generate Stop */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

        hi2c->State = HAL_I2C_STATE_READY;

        return 0;
    }
    else
    {
        hi2c->State = HAL_I2C_STATE_BUSY;
        return -1;
    }
}

int8_t I2C_Master_Receive(i2cHandler *hi2c, uint8_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /* Init tickstart for timeout management */
    uint32_t tickstart = GetTick();

    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        /* Wait until BUSY flag is reset */
        if (I2C_WaitOnBUSYFlagUntilTimeout(hi2c, tickstart, Timeout))
            return -1;

        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
        {
            hi2c->State = HAL_I2C_STATE_READY;
            hi2c->ErrorCode |= HAL_I2C_ERROR_DISABLE;
            return -1;
        }

        /* Disable Pos */
        CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

        hi2c->State = HAL_I2C_STATE_BUSY_RX;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
        hi2c->pBuffPtr = pData;
        hi2c->XferCount = Size;
        hi2c->XferSize = hi2c->XferCount;

        /* Enable Acknowledge */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

        /* Generate Start */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);

        /* Wait until SB flag is set */
        if (I2C_WaitOnSBFlagUntilTimeout(hi2c, tickstart, Timeout))
            return -1;

        /* Send slave address (R)*/
        LL_I2C_TransmitData8(hi2c->Instance, ((uint8_t)((DevAddress) | I2C_OAR1_ADD0)));

        /* Wait until ADDR flag is set */
        if (I2C_WaitOnADDRFlagUntilTimeout(hi2c, tickstart, Timeout))
            return -1;

        if (hi2c->XferSize == 0U)
        {
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(hi2c->Instance);

            /* Generate Stop */
            SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
        }
        else if (hi2c->XferSize == 1U)
        {
            /* Disable Acknowledge */
            CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

            /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
            software sequence must complete before the current byte end of transfer */
            __disable_irq();

            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(hi2c->Instance);

            /* Generate Stop */
            SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

            /* Re-enable IRQs */
            __enable_irq();
        }
        else if (hi2c->XferSize == 2U)
        {
            /* Enable Pos */
            SET_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

            /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
            software sequence must complete before the current byte end of transfer */
            __disable_irq();

            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(hi2c->Instance);

            /* Disable Acknowledge */
            CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

            /* Re-enable IRQs */
            __enable_irq();
        }
        else
        {
            /* Enable Acknowledge */
            SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(hi2c->Instance);
        }

        while (hi2c->XferCount > 0U)
        {
            if (hi2c->XferCount <= 3U)
            {
                /* One byte */
                if (hi2c->XferCount == 1U)
                {
                    /* Wait until RXNE flag is set */
                    if (I2C_WaitOnRXNEFlagUntilTimeout(hi2c, tickstart, Timeout))
                        return -1;

                    /* Read data from DR */
                    *hi2c->pBuffPtr = LL_I2C_ReceiveData8(hi2c->Instance);

                    /* Increment Buffer pointer */
                    hi2c->pBuffPtr++;

                    /* Update counter */
                    hi2c->XferCount--;
                }
                /* Two bytes */
                else if (hi2c->XferCount == 2U)
                {
                    /* Wait until BTF flag is set */
                    if (I2C_WaitOnBTFFlagUntilTimeout(hi2c, tickstart, Timeout))
                        return -1;

                    /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                    software sequence must complete before the current byte end of transfer */
                    __disable_irq();

                    /* Generate Stop */
                    SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

                    /* Read data from DR */
                    *hi2c->pBuffPtr = LL_I2C_ReceiveData8(hi2c->Instance);

                    /* Increment Buffer pointer */
                    hi2c->pBuffPtr++;

                    /* Update counter */
                    hi2c->XferCount--;

                    /* Re-enable IRQs */
                    __enable_irq();

                    /* Read data from DR */
                    *hi2c->pBuffPtr = LL_I2C_ReceiveData8(hi2c->Instance);

                    /* Increment Buffer pointer */
                    hi2c->pBuffPtr++;

                    /* Update counter */
                    hi2c->XferCount--;
                }
                /* 3 Last bytes */
                else
                {
                    /* Wait until BTF flag is set */
                    if (I2C_WaitOnBTFFlagUntilTimeout(hi2c, tickstart, Timeout))
                        return -1;

                    /* Disable Acknowledge */
                    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

                    /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                    software sequence must complete before the current byte end of transfer */
                    __disable_irq();

                    /* Read data from DR */
                    *hi2c->pBuffPtr = LL_I2C_ReceiveData8(hi2c->Instance);

                    /* Increment Buffer pointer */
                    hi2c->pBuffPtr++;

                    /* Update counter */
                    hi2c->XferCount--;

                    /* Wait until BTF flag is set */
                    if (I2C_WaitOnBTFFlagUntilTimeout(hi2c, tickstart, Timeout))
                    {
                        /* Re-enable IRQs */
                        __enable_irq();
                        return -1;
                    }

                    /* Generate Stop */
                    SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

                    /* Read data from DR */
                    *hi2c->pBuffPtr = LL_I2C_ReceiveData8(hi2c->Instance);

                    /* Increment Buffer pointer */
                    hi2c->pBuffPtr++;

                    /* Update counter */
                    hi2c->XferCount--;

                    /* Re-enable IRQs */
                    __enable_irq();

                    /* Read data from DR */
                    *hi2c->pBuffPtr = LL_I2C_ReceiveData8(hi2c->Instance);

                    /* Increment Buffer pointer */
                    hi2c->pBuffPtr++;

                    /* Update counter */
                    hi2c->XferCount--;
                }
            }
            else
            {
                /* Wait until BTF flag is set */
                if (I2C_WaitOnBTFFlagUntilTimeout(hi2c, tickstart, Timeout))
                    return -1;

                /* Read data from DR */
                *hi2c->pBuffPtr = LL_I2C_ReceiveData8(hi2c->Instance);

                /* Increment Buffer pointer */
                hi2c->pBuffPtr++;

                /* Update counter */
                hi2c->XferCount--;
            }
        }
        hi2c->State = HAL_I2C_STATE_READY;

        return 0;
    }
    else
    {
        hi2c->State = HAL_I2C_STATE_BUSY;
        return -1;
    }
}