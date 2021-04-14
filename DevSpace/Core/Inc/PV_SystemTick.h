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
*       14APRI21    Lizhou        Add .c file, fix bugs?
*
*************************************************************************/
#ifndef __PVSYSTICK_H
#define __PVSYSTICK_H
#include "stm32f1xx.h"

void IncTick(void);
uint32_t GetTick(void);
void Delay(uint32_t Delay);

#endif /* __PVSYSTICK_H */