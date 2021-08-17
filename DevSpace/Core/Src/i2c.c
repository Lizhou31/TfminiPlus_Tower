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
*       17AUG21     Lizhou        Optimize and fix some Error
*
*************************************************************************/
#include "i2c.h"
#define GetTick(void) GetTick(void)

void i2chandler_init(I2CHandler *i2chandler, I2C_TypeDef *Instance)
{
    i2chandler->Instance = Instance;
    i2chandler->ErrorCode = I2C_ERROR_NONE;
    i2chandler->XferCount = 0;
}

static uint8_t I2C_WaitOnBUSYFlagUntilTimeout(I2CHandler *i2chandler, uint32_t tickstart, uint32_t Timeout)
{
    while (LL_I2C_IsActiveFlag_BUSY(i2chandler->Instance))
    {
        /* Check for the Timeout */
        if (Timeout < I2C_MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                i2chandler->ErrorCode = I2C_ERROR_TIMEOUT;
                return ERROR;
            }
        }
    }
    return SUCCESS;
}

static uint8_t I2C_WaitOnSBFlagUntilTimeout(I2CHandler *i2chandler, uint32_t tickstart, uint32_t Timeout)
{
    while (!LL_I2C_IsActiveFlag_SB(i2chandler->Instance))
    {
        /* Check for the Timeout */
        if (Timeout < I2C_MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                i2chandler->ErrorCode = I2C_ERROR_TIMEOUT;
                if (READ_BIT(i2chandler->Instance->CR1, I2C_CR1_START) == I2C_CR1_START)
                    i2chandler->ErrorCode = I2C_WRONG_START;
                return ERROR;
            }
        }
    }
    return SUCCESS;
}

static uint8_t I2C_WaitOnADDRFlagUntilTimeout(I2CHandler *i2chandler, uint32_t tickstart, uint32_t Timeout)
{
    while (!LL_I2C_IsActiveFlag_ADDR(i2chandler->Instance))
    {
        /* Check if a NACK is detected */
        if (LL_I2C_IsActiveFlag_AF(i2chandler->Instance))
        {
            /* Generate Stop */
            LL_I2C_GenerateStopCondition(i2chandler->Instance);

            /* Clear AF Flag */
            LL_I2C_ClearFlag_AF(i2chandler->Instance);

            i2chandler->ErrorCode = I2C_ERROR_AF;
            return ERROR;
        }

        /* Check for the Timeout */
        if (Timeout < I2C_MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                i2chandler->ErrorCode = I2C_ERROR_TIMEOUT;
                return ERROR;
            }
        }
    }
    return SUCCESS;
}

static uint8_t I2C_WaitOnTXEFlagUntilTimeout(I2CHandler *i2chandler, uint32_t tickstart, uint32_t Timeout)
{
    while (!LL_I2C_IsActiveFlag_TXE(i2chandler->Instance))
    {
        /* Check if a NACK is detected */
        if (LL_I2C_IsActiveFlag_AF(i2chandler->Instance))
        {
            /* Generate Stop */
            LL_I2C_GenerateStopCondition(i2chandler->Instance);

            /* Clear AF Flag */
            LL_I2C_ClearFlag_AF(i2chandler->Instance);

            i2chandler->ErrorCode = I2C_ERROR_AF;
            return ERROR;
        }

        /* Check for the Timeout */
        if (Timeout < I2C_MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                i2chandler->ErrorCode = I2C_ERROR_TIMEOUT;
                return ERROR;
            }
        }
    }
    return SUCCESS;
}

static uint8_t I2C_WaitOnBTFFlagUntilTimeout(I2CHandler *i2chandler, uint32_t tickstart, uint32_t Timeout)
{
    while (!LL_I2C_IsActiveFlag_BTF(i2chandler->Instance))
    {
        /* Check if a NACK is detected */
        if (LL_I2C_IsActiveFlag_AF(i2chandler->Instance))
        {
            /* Generate Stop */
            LL_I2C_GenerateStopCondition(i2chandler->Instance);

            /* Clear AF Flag */
            LL_I2C_ClearFlag_AF(i2chandler->Instance);

            i2chandler->ErrorCode = I2C_ERROR_AF;
            return ERROR;
        }
        /* Check for the Timeout */
        if (Timeout < I2C_MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                i2chandler->ErrorCode = I2C_ERROR_TIMEOUT;
                return ERROR;
            }
        }
    }
    return SUCCESS;
}

static uint8_t I2C_WaitOnRXNEFlagUntilTimeout(I2CHandler *i2chandler, uint32_t tickstart, uint32_t Timeout)
{
    while (LL_I2C_IsActiveFlag_RXNE(i2chandler->Instance))
    {
        if (LL_I2C_IsActiveFlag_STOP(i2chandler->Instance))
        {
            /* Clear STOP Flag */
            LL_I2C_ClearFlag_STOP(i2chandler->Instance);

            i2chandler->ErrorCode = I2C_ERROR_NONE;
            return ERROR;
        }
        /* Check for the Timeout */
        if (Timeout < I2C_MAX_DELAY)
        {
            if ((GetTick() - tickstart) > Timeout)
            {
                i2chandler->ErrorCode = I2C_ERROR_TIMEOUT;
                return ERROR;
            }
        }
    }
    return SUCCESS;
}

uint8_t I2C_Master_Transmit(I2CHandler *i2chandler, uint16_t DevAddress, uint8_t *pData, uint16_t size, uint32_t Timeout)
{
    uint32_t tickstart = GetTick();

    /* Wait until BUSY flag is reset */
    if (I2C_WaitOnBUSYFlagUntilTimeout(i2chandler, tickstart, Timeout))
        return ERROR;

    /* Check if the I2C is already enabled */
    if (!LL_I2C_IsEnabled(i2chandler->Instance))
    {
        LL_I2C_Enable(i2chandler->Instance);
    }

    /* Disable Pos */
    LL_I2C_DisableBitPOS(i2chandler->Instance);

    i2chandler->XferCount = size;

    /* Generate Start */
    LL_I2C_GenerateStartCondition(i2chandler->Instance);

    /* Wait until SB flag is set */
    if (I2C_WaitOnSBFlagUntilTimeout(i2chandler, tickstart, Timeout))
        return ERROR;

    /* Send slave address (W)*/
    LL_I2C_TransmitData8(i2chandler->Instance, ((uint8_t)((DevAddress) & (uint8_t)(~I2C_OAR1_ADD0))));

    /* Wait until ADDR flag is set */
    if (I2C_WaitOnADDRFlagUntilTimeout(i2chandler, tickstart, Timeout))
        return ERROR;

    /* Clear ADDR flag */
    LL_I2C_ClearFlag_ADDR(i2chandler->Instance);

    /* Wait until TXE flag is set */
    if (I2C_WaitOnTXEFlagUntilTimeout(i2chandler, tickstart, Timeout))
        return ERROR;

    while (i2chandler->XferCount > 0U)
    {
        LL_I2C_TransmitData8(i2chandler->Instance, *pData);

        /* Increment Buffer pointer */
        pData++;

        /* Update counter */
        i2chandler->XferCount--;

        if (I2C_WaitOnBTFFlagUntilTimeout(i2chandler, tickstart, Timeout))
            return ERROR;
    }

    /* Generate Stop */
    LL_I2C_GenerateStopCondition(i2chandler->Instance);

    return SUCCESS;
}

uint8_t I2C_Master_Receive(I2CHandler *i2chandler, uint16_t DevAddress, uint8_t *pData, uint16_t size, uint32_t Timeout)
{
    uint32_t tickstart = GetTick();

    /* Wait until BUSY flag is reset */
    if (I2C_WaitOnBUSYFlagUntilTimeout(i2chandler, tickstart, Timeout))
        return ERROR;

    /* Check if the I2C is already enabled */
    if (!LL_I2C_IsEnabled(i2chandler->Instance))
    {
        LL_I2C_Enable(i2chandler->Instance);
    }

    /* Disable Pos */
    LL_I2C_DisableBitPOS(i2chandler->Instance);

    i2chandler->XferCount = size;

    /* Enable Acknowledge */
    SET_BIT(i2chandler->Instance->CR1, I2C_CR1_ACK);

    /* Generate Start */
    LL_I2C_GenerateStartCondition(i2chandler->Instance);

    /* Wait until SB flag is set */
    if (I2C_WaitOnSBFlagUntilTimeout(i2chandler, tickstart, Timeout))
        return ERROR;

    /* Send slave address (R)*/
    LL_I2C_TransmitData8(i2chandler->Instance, ((uint8_t)((DevAddress) | I2C_OAR1_ADD0)));

    /* Wait until ADDR flag is set */
    if (I2C_WaitOnADDRFlagUntilTimeout(i2chandler, tickstart, Timeout))
        return ERROR;

    if (i2chandler->XferCount == 0U)
    {
        /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(i2chandler->Instance);

        /* Generate Stop */
        LL_I2C_GenerateStopCondition(i2chandler->Instance);
    }
    else if (i2chandler->XferCount == 1U)
    {
        /* Disable Acknowledge */
        CLEAR_BIT(i2chandler->Instance->CR1, I2C_CR1_ACK);

        /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
        software sequence must complete before the current byte end of transfer */
        __disable_irq();

        /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(i2chandler->Instance);

        /* Generate Stop */
        LL_I2C_GenerateStopCondition(i2chandler->Instance);

        /* Re-enable IRQs */
        __enable_irq();
    }
    else if (i2chandler->XferCount == 2U)
    {
        /* Enable Pos */
        LL_I2C_EnableBitPOS(i2chandler->Instance);

        /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
            software sequence must complete before the current byte end of transfer */
        __disable_irq();

        /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(i2chandler->Instance);

        /* Disable Acknowledge */
        CLEAR_BIT(i2chandler->Instance->CR1, I2C_CR1_ACK);

        /* Re-enable IRQs */
        __enable_irq();
    }
    else
    {
        /* Enable Acknowledge */
        SET_BIT(i2chandler->Instance->CR1, I2C_CR1_ACK);

        /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(i2chandler->Instance);
    }

    while (i2chandler->XferCount > 0U)
    {
        if (i2chandler->XferCount <= 3U)
        {
            /* One byte */
            if (i2chandler->XferCount == 1U)
            {
                /* Wait until RXNE flag is set */
                if (I2C_WaitOnRXNEFlagUntilTimeout(i2chandler, tickstart, Timeout))
                    return ERROR;

                /* Read data from DR */
                *pData = LL_I2C_ReceiveData8(i2chandler->Instance);

                /* Increment Buffer pointer */
                pData++;

                /* Update counter */
                i2chandler->XferCount--;
            }
            /* Two bytes */
            else if (i2chandler->XferCount == 2U)
            {
                /* Wait until BTF flag is set */
                if (I2C_WaitOnBTFFlagUntilTimeout(i2chandler, tickstart, Timeout))
                    return ERROR;

                /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                    software sequence must complete before the current byte end of transfer */
                __disable_irq();

                /* Generate Stop */
                LL_I2C_GenerateStopCondition(i2chandler->Instance);

                /* Read data from DR */
                *pData = LL_I2C_ReceiveData8(i2chandler->Instance);

                /* Increment Buffer pointer */
                pData++;

                /* Update counter */
                i2chandler->XferCount--;

                /* Re-enable IRQs */
                __enable_irq();

                /* Read data from DR */
                *pData = LL_I2C_ReceiveData8(i2chandler->Instance);

                /* Increment Buffer pointer */
                pData++;

                /* Update counter */
                i2chandler->XferCount--;
            }
            /* 3 Last bytes */
            else
            {
                /* Wait until BTF flag is set */
                if (I2C_WaitOnBTFFlagUntilTimeout(i2chandler, tickstart, Timeout))
                    return ERROR;

                /* Disable Acknowledge */
                CLEAR_BIT(i2chandler->Instance->CR1, I2C_CR1_ACK);

                /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                    software sequence must complete before the current byte end of transfer */
                __disable_irq();

                /* Read data from DR */
                *pData = LL_I2C_ReceiveData8(i2chandler->Instance);

                /* Increment Buffer pointer */
                pData++;

                /* Update counter */
                i2chandler->XferCount--;

                /* Wait until BTF flag is set */
                if (I2C_WaitOnBTFFlagUntilTimeout(i2chandler, tickstart, Timeout))
                {
                    /* Re-enable IRQs */
                    __enable_irq();
                    return ERROR;
                }

                /* Generate Stop */
                LL_I2C_GenerateStopCondition(i2chandler->Instance);

                /* Read data from DR */
                *pData = LL_I2C_ReceiveData8(i2chandler->Instance);

                /* Increment Buffer pointer */
                pData++;

                /* Update counter */
                i2chandler->XferCount--;

                /* Re-enable IRQs */
                __enable_irq();

                /* Read data from DR */
                *pData = LL_I2C_ReceiveData8(i2chandler->Instance);

                /* Increment Buffer pointer */
                pData++;

                /* Update counter */
                i2chandler->XferCount--;
            }
        }
        else
        {
            /* Wait until BTF flag is set */
            if (I2C_WaitOnBTFFlagUntilTimeout(i2chandler, tickstart, Timeout))
                return ERROR;

            /* Read data from DR */
            *pData = LL_I2C_ReceiveData8(i2chandler->Instance);

            /* Increment Buffer pointer */
            pData++;

            /* Update counter */
            i2chandler->XferCount--;
        }
    }
    return SUCCESS;
}
