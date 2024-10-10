// STM32F401RE_RCC.h
// Header for RCC functions
// Victoria Parizot
// vparizot@g.hmc.edu
// 10/5/2024

#ifndef STM32L4_RCC_H
#define STM32L4_RCC_H

#include <stdint.h>
#include <stm32l432xx.h>

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void configurePLL();
void configureClock();

#endif