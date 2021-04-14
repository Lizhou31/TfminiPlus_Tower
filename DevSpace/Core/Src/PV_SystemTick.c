/***********************************************************************
* FILENAME :        PV_SystemTick.c
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
*       14APRI21    Lizhou        Add .c file, fix bugs?
*
*************************************************************************/
#include "PV_SystemTick.h"

__IO uint32_t msTick;

void IncTick(void)
{
    msTick++;
}

uint32_t GetTick(void)
{
    return msTick;
}

void Delay(uint32_t Delay)
{
    uint32_t tickstart = GetTick();
    uint32_t wait = Delay;

    while ((GetTick() - tickstart) < wait)
    {
    }
}