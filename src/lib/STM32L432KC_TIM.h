// STM32F401RE_TIM.h
// Header for TIM functions
// Victoria Parizot
// vparizot@g.hmc.edu
// 10/5/2024
#ifndef STM32L4_TIM_H
#define STM32L4_TIM_H

#include <stdint.h> // Include stdint header
#include <stm32l432xx.h>
#include "STM32L432KC_GPIO.h"

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void initTIM(TIM_TypeDef * TIMx);
void initcntTIM(TIM_TypeDef * TIMx);
void delay_millis(TIM_TypeDef * TIMx, uint32_t ms);
void delay_micros(TIM_TypeDef * TIMx, uint32_t us);

#endif