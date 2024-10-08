// STM32F401RE_TIM.c
// TIM functions

#include "STM32L432KC_TIM.h"
#include "STM32L432KC_RCC.h"


//void initTIM(TIM_TypeDef * TIMx){
//    //prescaler register:  to get timer freq = 100 kHz
//    TIMx->PSC = 39; //|= (0b1111<<0) ; //(pg. 959 for TIM16)

//    // Generate an update event to update prescaler value
//    TIMx->EGR |= 1;
    
//    //TIMx->ARR=5000; // set ARR

//    // pg. 950 enable CEN (counter enable)
//    TIMx->CR1 |= 1; //CEN = 1

//    TIMx->CR1 |= (1<<7); //autoreload reg en
    
//    //reset counter
//    TIMx->CNT = 0;
//}


void initTIM(TIM_TypeDef * TIMx){
  // Set prescaler to give 1 ms time base
  uint32_t psc_div = (uint32_t) ((SystemCoreClock/1e3));

  // Set prescaler division factor
  TIMx->PSC = (psc_div - 1);
  // Generate an update event to update prescaler value
  TIMx->EGR |= 1;
  // Enable counter
  TIMx->CR1 |= 1; // Set CEN = 1
}

void initcntTIM(TIM_TypeDef * TIMx){
  // Set prescaler to give 1 micros time base
  uint32_t psc_div = (uint32_t) ((SystemCoreClock/1e6));

  // Set prescaler division factor
  TIMx->PSC = (psc_div - 1);
  // Generate an update event to update prescaler value
  TIMx->EGR |= 1;
  // Enable counter
  TIMx->CR1 |= 1; // Set CEN = 1
}

void delay_millis(TIM_TypeDef * TIMx, uint32_t ms){
  TIMx->ARR = ms;     // Set timer max count
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count

   while(!(TIMx->SR & 1)); // Wait for UIF to go high
}