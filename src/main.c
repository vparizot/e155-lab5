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

#define sigA PA2
#define sigB PA3


//#define DELAY_TIM TIM2

#define cntTIM TIM16

// Define Global variables
int speed; // where 0 = cw and 1 = ccw
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
    // Configure flash to add waitstates to avoid timing errors
    configureFlash();

    // CONFIGURE CLOCKS IN RCC: (do we still have to Setup the PLL and switch clock source to the PLL)
    configureClock();
 
    //clock source control
    RCC->CFGR |= (1<<10); //bits 10:8 (PPRE1) where 0xx is HCLK not divided
    RCC->CFGR |= (1<<7) ; // AHB Prescaler, (HPRE[3:0] = 0xxx)

    //enable timer (pg 245)
    RCC->APB2ENR |= (1<<17); //enable timer 16

    //initialize timer @ 100kHz
    initTIM(cntTIM);

    /////////////////////////////////////////////////////////
    ///////// code from interrupt tutorial
    ////////////////////////////////////////////////////////
    ////////initialize timer 
    //////RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    //////initTIM(cntTIM);

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
    SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI2, 0b000); // Select PA2
    SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI3, 0b000); // Select PA3

    //////////////////////////////////////
    //////// INTERRUPTS (HW external interrupt procedure described pg. 327)
    //////////////////////////////////////
    // Enable interrupts globally
    __enable_irq();

    //Interrupts for SigA = PA2 (pg. 330)
    // TODO: Configure interrupt for falling AND RISING edge of GPIO pin for button
    // 1. Configure mask bit
    EXTI->IMR1 |= (1 << gpioPinOffset(sigA)); // Configure the mask bit
    // 2. ENABLE rising edge trigger (enable interrupt generation)
    EXTI->RTSR1 |= (1 << gpioPinOffset(sigA));// 
    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(sigA));// Enable falling edge trigger
    // 4. Turn on EXTI interrupt in NVIC_ISER (321
    NVIC->ISER[0] |= (1 << EXTI2_IRQn);

    //Interrupts for SigB = PA3
    // TODO: Configure interrupt for falling AND RISING edge of GPIO pin for button
    // 1. Configure mask bit
    EXTI->IMR2 |= (1 << gpioPinOffset(sigB)); // Configure the mask bit
    // 2. ENABLE rising edge trigger (enable interrupt generation)
    EXTI->RTSR2 |= (1 << gpioPinOffset(sigB));// 
    // 3. Enable falling edge trigger
    EXTI->FTSR2 |= (1 << gpioPinOffset(sigB));// Enable falling edge trigger
    // 4. Turn on EXTI interrupt in NVIC_ISER
    NVIC->ISER[0] |= (1 << EXTI3_IRQn);

    while (1){
      // calc speed (delT between rising edges)
      speed = abs(posA-posB); //TODO: make expressions based on posA, posB
      printf("Current speed: %d \n", speed);

      // calc direction
      if (posA > posB){
        printf("Current direction: ccw \n");
      } else{
        printf("Current direction: cw \n");
      }
    }
}

// rising/falling edge of A
void EXTI2_IRQHandler(void){
    // Check that the button was what triggered our interrupt
    if (EXTI->PR1 & (1 << 0)){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << 0);

        //check counter and set as PosB PosB = CNT
        posA = cntTIM->CNT;

        //reset counter CNT = 0
        cntTIM->CNT = 0;
    }
}

//rising/falling edge of B
void EXTI3_IRQHandler(void){
    // Check that the button was what triggered our interrupt
    if (EXTI->PR1 & (1 <<0 )){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 <<0 );

        //check counter and set as PosB PosB = CNT
        posB = cntTIM->CNT;

        //reset counter CNT = 0
        cntTIM->CNT = 0;
    }
}



#endif // MAIN_H