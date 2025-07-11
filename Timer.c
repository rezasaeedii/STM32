#include "stm32f10x.h"

// define global parameters //
volatile int counter = 0;
volatile int is_on = 0, onfire = 0, prev_A2 = 0;

// define functions //
void TIM2_IRQHandler (void);


// main project //
int main(void) {
	
	  //----------------------------------------------------------// set clock
	
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;           // set clock for GPIO A
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;           // set clock for GPIO C
	  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; 					// set clock for AFIO
	  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;           // set clock for TIM2
	
	  //----------------------------------------------------------// set GPIO
	
	  GPIOA->CRL &= ~(0xF << 8 );                   // reset PA2
	  GPIOA->CRL |=  (0x8  << 8 );                   // MODE0 = 00 , CNF0 = 10  set as input
	  GPIOA->ODR &= ~(1 << 2 );
	  
	  GPIOC->CRH &= ~(0xF << 20 );                  // reset PC13
	  GPIOC->CRH |=  (0x3 << 20 );                  // MODE0 = 11 , CNF0 = 00  set as output
	
	  
		//----------------------------------------------------------// set Timer
	  
	  TIM2->PSC   = 7200 - 1;                       // set clock 10KHz
	  TIM2->ARR   = 10 - 1;                         // timescale = 1ms
	  TIM2->RCR   = 0;
	  TIM2->CR1  |= (1 << 0);                       // counter enable
    TIM2->DIER |= (1 << 0);                       // enable intrupt
	  
    //----------------------------------------------------------//
	  
	  NVIC_EnableIRQ(TIM2_IRQn);                    // set function for all part       
		
    //----------------------------------------------------------//
	
    while(1) {
			  int cur_A2 = (GPIOA->IDR & (1 << 2)) ? 1 : 0;       // set current state
			  
        if (cur_A2 && !prev_A2) {
        is_on = 1; counter = 0; onfire = 0;
        }
				prev_A2 = cur_A2;                                   // set next state
			  
				if((counter >= 5000) && (is_on == 1) && (onfire == 0)){
						GPIOC->BSRR |= (1 << 13);
					  counter = 0;
					  is_on = 0;
					  onfire = 1;
				}
				if((counter >= 2000) && (onfire == 1)){
						GPIOC->BRR |= (1 << 13);
					  counter = 0;
					  onfire = 0;
					  is_on = 0;
				}
				
    }
}



// define functions //

void TIM2_IRQHandler (void){ 
		if(TIM2->SR & (1 << 0)){                       // SR is a register for check status of intrupt
			  counter ++ ;
			 TIM2->SR &= ~(1 << 0);                     // reset status to default
		}
}

