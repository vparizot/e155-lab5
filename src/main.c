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

//Main code for lab 5, using interrupts to read motor speed & direction

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
float speed, speedavg;
int dir;
int window[9]; 
int windowIncr;
int delCountAvg;
int delCount;

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
    //enable timer (pg 245)
    RCC->APB2ENR |= (1<<17); //enable timer 16
    RCC->APB2ENR |= RCC_APB2ENR_TIM16EN; //enable tim16
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; //enable tim2

    //initialize timer @ 100kHz
    initcntTIM(cntTIM); // @100kHz
    initTIM(printTIM); // 1ms

    // Enable sigA and sigB as inputs
    gpioEnable(GPIO_PORT_A);
    pinMode(sigA, GPIO_INPUT);
    pinMode(sigB, GPIO_INPUT);
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD2, 0b01); // Set PA2 as pull-up
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD3, 0b01); // Set PA3 as pull-up


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
    // Configure interrupt for falling AND RISING edge of GPIO pin for button
    // 1. Configure mask bit
    EXTI->IMR1 |= (1 << gpioPinOffset(sigA)); // Configure the mask bit
    // 2. ENABLE rising edge trigger (enable interrupt generation)
    EXTI->RTSR1 |= (1 << gpioPinOffset(sigA));// 
    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(sigA));// Enable falling edge trigger
    // 4. Turn on EXTI interrupt in NVIC_ISER (321
    //NVIC->ISER[0] |= (1 << EXTI2_IRQn);

    //Interrupts for SigB = PA8
    // Configure interrupt for falling AND RISING edge of GPIO pin for button
    // 1. Configure mask bit
    EXTI->IMR1 |= (1 << gpioPinOffset(sigB)); // Configure the mask bit
    // 2. ENABLE rising edge trigger (enable interrupt generation)
    EXTI->RTSR1 |= (1 << gpioPinOffset(sigB));// 
    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(sigB));// Enable falling edge trigger
    // 4. Turn on EXTI interrupt in NVIC_ISER
    NVIC->ISER[0] |= (1 << EXTI9_5_IRQn);

    while (1){
      delCountAvg = 0;
      for (int i = 0; i < 10; i++){
        delCountAvg += window[i] / 10;
      }
      
      speedavg = 1.0/(120.0*4.0*(delCountAvg/1000000.0));

      //speed = 1.0/(120.0*4.0*(delCount/1000000.0));
      //printf("Current avg del: %d \n", delCountAvg);
      //printf("Current del: %d \n", delCount);

      printf("Current speed: %f [rev/s]\n", speedavg);
   
      //calc direction
      //if (dir == 0){
      //  printf("Current direction: ccw \n");
      //} else {
      //  printf("Current direction: cw \n");
      //}

      delay_millis(printTIM, 1000);
    }
}

// rising/falling edge of A
void EXTI9_5_IRQHandler(void){
    // Check that the button was what triggered our interrupt
    int B = digitalRead(sigB);
    int A = digitalRead(sigA);
    if (EXTI->PR1 & (1 <<8)){ //SIGNAL B
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 <<8);

///////////////////////////// 
        if (((B ==1) && (A ==1)) || ((B==0) && (A==0))){
          delCount = - cntTIM->CNT;
          windowIncr++;
          window[windowIncr%10] = delCount;
        }
//////////////////////////////////

        cntTIM->CNT = 0; //reset counter CNT = 0
        
        cntTIM->EGR |= 1; // Generate an update event
    }
   
    if (EXTI->PR1 & (1 << 6)){ //SIGNAL A
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << 6);
 ////////////////////////
        if (((B ==1) && (A ==1)) || ((B==0) && (A==0))){
          delCount =  cntTIM->CNT;
          windowIncr++;
          window[windowIncr%10] = delCount;
        }
 ///////////////////////////////

        //delCount = cntTIM->CNT;
        //windowIncr++;
        //window[windowIncr%10] = delCount;
      
     
        //if (digitalRead(sigB)){
        // dir = 1;//check if B is high
        //} else dir = 0;

        //reset counter CNT = 0
        cntTIM->CNT = 0;         
        cntTIM->EGR |= 1; // Generate an update event
    }
}

#endif // MAIN_H