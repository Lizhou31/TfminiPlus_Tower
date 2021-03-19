/***********************************************************************
* FILENAME :        PV_SystemTick.h
*
* DESCRIPTION :
*       Private System Ticks Lib.
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
#ifndef __PVSYSTICK_H
#define __PVSYSTICK_H
#include "stm32f1xx.h"
uint32_t msTick;

__STATIC_INLINE void IncTick(void)
{
    msTick++;
}

__STATIC_INLINE uint32_t GetTick(void)
{
    return msTick;
}

#endif /* __PVSYSTICK_H */