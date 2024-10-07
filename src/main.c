/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Generic application start

*/

// main.h
// Victoria Parizot
// vparizot@g.hmc.edu
// 10/5/2024

#ifndef MAIN_H
#define MAIN_H

#include "lib/STM32L432KC.h"
#include <stm32l432xx.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// Custom defines
///////////////////////////////////////////////////////////////////////////////

#define sigA PA6
#define sigB PA8


//#define DELAY_TIM TIM2

#define cntTIM TIM16
#define printTIM TIM2

// Define Global variables
float speed;
int delCount; // where 0 = cw and 1 = ccw
int posA, posB;

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}


int main(void) {
    printf("Starting... \n");
    //// Configure flash to add waitstates to avoid timing errors
    //configureFlash();

    //// CONFIGURE CLOCKS IN RCC: (do we still have to Setup the PLL and switch clock source to the PLL)
    //configureClock();
 
    ////clock source control
    //RCC->CFGR |= (1<<10); //bits 10:8 (PPRE1) where 0xx is HCLK not divided
    //RCC->CFGR |= (1<<7) ; // AHB Prescaler, (HPRE[3:0] = 0xxx)

    //enable timer (pg 245)
    RCC->APB2ENR |= (1<<17); //enable timer 16
    RCC->APB2ENR |= RCC_APB2ENR_TIM16EN; //enable tim16
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; //enable tim2

    //initialize timer @ 100kHz
    initcntTIM(cntTIM); // @100kHz
    initTIM(printTIM); //1ms


    // Enable sigA and sigB as inputs
    gpioEnable(GPIO_PORT_A);
    pinMode(sigA, GPIO_INPUT);
    pinMode(sigB, GPIO_INPUT);
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD2, 0b01); // Set PA2 as pull-up
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD3, 0b01); // TODO Set PA3 as pull-up

    // TODO
    // 1. Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // 2. Configure EXTICR for the input button interrupt (pg. 328)
    SYSCFG->EXTICR[2] |= _VAL2FLD(SYSCFG_EXTICR2_EXTI6, 0b000); // Select PA6
    SYSCFG->EXTICR[3] |= _VAL2FLD(SYSCFG_EXTICR3_EXTI8, 0b000); // Select PA8

    //////////////////////////////////////
    //////// INTERRUPTS (HW external interrupt procedure described pg. 327)
    //////////////////////////////////////
    // Enable interrupts globally
    __enable_irq();

    //Interrupts for SigA = PA6 (pg. 330)
    // TODO: Configure interrupt for falling AND RISING edge of GPIO pin for button
    // 1. Configure mask bit
    EXTI->IMR1 |= (1 << gpioPinOffset(sigA)); // Configure the mask bit
    // 2. ENABLE rising edge trigger (enable interrupt generation)
    EXTI->RTSR1 |= (1 << gpioPinOffset(sigA));// 
    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(sigA));// Enable falling edge trigger
    // 4. Turn on EXTI interrupt in NVIC_ISER (321
    //NVIC->ISER[0] |= (1 << EXTI2_IRQn);

    //Interrupts for SigB = PA8
    // TODO: Configure interrupt for falling AND RISING edge of GPIO pin for button
    // 1. Configure mask bit
    EXTI->IMR1 |= (1 << gpioPinOffset(sigB)); // Configure the mask bit
    // 2. ENABLE rising edge trigger (enable interrupt generation)
    EXTI->RTSR1 |= (1 << gpioPinOffset(sigB));// 
    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(sigB));// Enable falling edge trigger
    // 4. Turn on EXTI interrupt in NVIC_ISER
    NVIC->ISER[0] |= (1 << EXTI9_5_IRQn);

    while (1){
      // calc speed (delT between rising edges)
      delCount = abs(posA-posB); //TODO: make expressions based on posA, posB

      speed = 1.0/(120.0*4.0*(delCount/1000000.0));

      printf("Current del: %d \n", delCount);
      printf("Current speed: %f \n", speed);
      // calc direction
      if (posA > posB){
        printf("Current direction: ccw \n");
      } else {
        printf("Current direction: cw \n");
      }

      delay_millis(printTIM, 1000);
    }
}

// rising/falling edge of A
void EXTI9_5_IRQHandler(void){
    // Check that the button was what triggered our interrupt
     
    //B
    if (EXTI->PR1 & (1 <<8)){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 <<8 );

        //check counter and set as PosB PosB = CNT
        posB = cntTIM->CNT;

        //reset counter CNT = 0
        cntTIM->CNT = 0;

         // Generate an update event to update prescaler value
        cntTIM->EGR |= 1;
    }
    
    //A
    if (EXTI->PR1 & (1 << 6)){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << 6);

        //check counter and set as PosA PosA = CNT
        posA = cntTIM->CNT;

        //reset counter CNT = 0
        cntTIM->CNT = 0;         
       // Generate an update event to update prescaler value
        cntTIM->EGR |= 1;
    }
}




#endif // MAIN_H