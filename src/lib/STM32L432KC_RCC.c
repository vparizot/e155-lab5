// STM32F401RE_RCC.c
// Source code for RCC functions

#include "STM32L432KC_RCC.h"

void configurePLL() {
   // Set clock to 4 MHz
   // Output freq = (src_clk) * (N/M) / R
   // (4 MHz) * (16/1) / 16 = 4mhz
   // M: 1, N: 80, R: 4
   // Use MSI as PLLSRC

    // Turn off PLL
    RCC->CR &= ~(1 << 24);
    
    // Wait till PLL is unlocked (e.g., off)
    while ((RCC->CR >> 25 & 1) != 0);

    // Load configuration
    // Set PLL SRC to MSI
    RCC->PLLCFGR |= (1 << 0);
    RCC->PLLCFGR &= ~(1 << 1);

    // Set PLLN
    RCC->PLLCFGR &= ~(0b11111111 << 8); // Clear all bits of PLLN
    RCC->PLLCFGR |= (0b10000 << 8); // |= 16
    
    // Set PLLM
    RCC->PLLCFGR &= ~(0b111 << 4);  // Clear all bits
    
    // Set PLLR = 2
    RCC->PLLCFGR &= ~(1<<26);
    RCC->PLLCFGR |= (1<<25);
    
    // Enable PLLR output
    RCC->PLLCFGR |= (1 << 24);

    // Enable PLL
    RCC->CR |= (1 << 24);
    
    // Wait until PLL is locked
    while ((RCC->CR >> 25 & 1) != 1);

 
   
////////////////////////////////////////////////////////////
//////////// code from interrupt tutorial
////////////////////////////////////////////////////////////

   //// Set clock to 80 MHz
   //// Output freq = (src_clk) * (N/M) / P
   //// (4 MHz) * (80/2) * 2  = 80 MHz
   //// M:, N:, P:
   //// Use HSI as PLLSRC

   //RCC->CR &= ~_FLD2VAL(RCC_CR_PLLON, RCC->CR); // Turn off PLL
   //while (_FLD2VAL(RCC_CR_PLLRDY, 1) != 0); // Wait till PLL is unlocked (e.g., off)

   //// Load configuration
   //RCC->PLLCFGR |= _VAL2FLD(RCC_PLLCFGR_PLLSRC, RCC_PLLCFGR_PLLSRC_MSI);
   //RCC->PLLCFGR |= _VAL2FLD(RCC_PLLCFGR_PLLM, 0b001); // M = 2
   //RCC->PLLCFGR |= _VAL2FLD(RCC_PLLCFGR_PLLN, 80);    // N = 80
   //RCC->PLLCFGR |= _VAL2FLD(RCC_PLLCFGR_PLLR, 0b00);  // R = 2
   //RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;                // Enable PLLCLK output

   //// Enable PLL and wait until it's locked
   //RCC->CR |= RCC_CR_PLLON;
   //while(_FLD2VAL(RCC_CR_PLLRDY, RCC->CR) == 0);
}

void configureClock(){
  // Configure and turn on PLL
  configurePLL();


  // Select PLL as clock source
  RCC->CFGR |= (0b11 << 0);
  while(!((RCC->CFGR >> 2) & 0b11));

////////////////////////////////////////////////////////////
//////////// code from interrupt tutorial
////////////////////////////////////////////////////////////
  //// Select PLL as clock source
  //RCC->CFGR = RCC_CFGR_SW_PLL | (RCC->CFGR & ~RCC_CFGR_SW);
  //while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

  //SystemCoreClockUpdate();
}