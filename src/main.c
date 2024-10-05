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

#define sigA PA5
#define sigB PA4
#define BUTTON_PIN PA5

//#define DELAY_TIM TIM2

#define cntTIM TIM2

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
  //initialize timer 
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
  initTIM(cntTIM);

  // Enable sigA as input
  gpioEnable(GPIO_PORT_A);
  pinMode(sigA, GPIO_INPUT);
 
  // Enable sigB as input
  gpioEnable(GPIO_PORT_A);
  pinMode(sigB, GPIO_INPUT);
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD2, 0b01); // Set PA2 as pull-up


  // Enable interrupts globally
  __enable_irq();

  // TODO: Configure interrupt for falling AND RISING edge of GPIO pin for button
  // 1. Configure mask bit
  EXTI->IMR1 |= (1 << gpioPinOffset(BUTTON_PIN)); // Configure the mask bit
  // 2. ENABLE rising edge trigger
  EXTI->RTSR1 &= ~(1 << gpioPinOffset(BUTTON_PIN));// TODO: SWITCH TO ENABLE Disable rising edge trigger
  // 3. Enable falling edge trigger
  EXTI->FTSR1 |= (1 << gpioPinOffset(BUTTON_PIN));// Enable falling edge trigger
  // 4. Turn on EXTI interrupt in NVIC_ISER
  NVIC->ISER[0] |= (1 << EXTI2_IRQn);

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