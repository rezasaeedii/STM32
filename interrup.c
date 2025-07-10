#include "stm32f10x.h"

// define functions //
void delay (int mydelay);
void EXTI9_5_IRQHandler (void);


// main project //
int main(void) {
	
	  //----------------------------------------------------------//
	
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;           // set clock for GPIO A
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;           // set clock for GPIO C
	  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; 					// set clock for AFIO
	
	  //----------------------------------------------------------//
	
    GPIOA->CRL &=~ (0xF << 7 * 4 );    					  // reset PA7
    GPIOA->CRL |=  (0x4 << 7 * 4 );            		// MODE7 = 00  , CNF7 = 01  
	  GPIOC->CRH &=~ (0xF << 5 * 4 );    					  // reset PC13
    GPIOC->CRH |=  (0x3 << 5 * 4 );            		// MODE13 = 11 , CNF13 = 00  
	  GPIOA->CRL &=~ (0xF << 0 );                   // reset PA0
	  GPIOA->CRL |= (0x2 << 0 );                   // MODE0 = 10 , CNF0 = 00  
		//----------------------------------------------------------//
	
		AFIO->EXTICR[1] &=~ AFIO_EXTICR2_EXTI7_PA;
		AFIO->EXTICR[1] |=  AFIO_EXTICR2_EXTI7_PA;
	  EXTI->IMR       |=  EXTI_IMR_MR7;
	  EXTI->EMR       &=~ EXTI_EMR_MR7;
	  EXTI->RTSR      &=~  EXTI_RTSR_TR7;           // negedge
	  EXTI->FTSR      |=  EXTI_FTSR_TR7;            // posedge
	  
    //----------------------------------------------------------//
	  
	  NVIC_EnableIRQ(EXTI9_5_IRQn);      
		
    //----------------------------------------------------------//
	
    while(1) {
			  GPIOC->BSRR = (1 << 13); 
        delay(5000); 
			  GPIOC->BRR = (1 << 13);  
        delay(5000);	
    }
}



// define functions //
void delay (int mydelay){
	while(mydelay--){
			for(int i = 0; i < 1000; i++);
	}
}

void EXTI9_5_IRQHandler  (void){ 
	if (EXTI -> PR & EXTI_PR_PR7){
		for(int i = 0; i < 15; i++){
			GPIOA->BSRR = (1 << 0); 
			delay(1000); 
			GPIOA->BRR = (1 << 0);  
			delay(1000);
			EXTI -> PR |= EXTI_PR_PR7;
	  }
  }
}

