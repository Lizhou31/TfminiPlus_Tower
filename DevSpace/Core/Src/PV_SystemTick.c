#include "PV_SystemTick.h"

uint32_t msTick;

void IncTick(void)
{
    msTick++;
}

uint32_t GetTick(void)
{
    return msTick;
}