#include "stm32f10x.h"

void delay (int mydelay){
    while(mydelay--){
        for(int i = 0; i < 1000; i++);
    }
}

int main(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // set clock for GPIO C
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // set clock for GPIO A

    
    GPIOC->CRH &= ~(0xF << ((13-8)*4)); //reset CNF13, MODE13,  because 13 > 8 so we use CRH
    GPIOC->CRH |= (0x2 << ((13-8)*4));  //MODE13 = 10 (2MHz) , CNF13 = 00 (Push-Pull) for GPIO C
    
    GPIOA->CRL &=~ (0xF << 7 * 4 );     // reset PA7
    GPIOA->CRL |= (0x3 << 7 * 4 );      // MODE7 = 11 (50MHz) , CNF7 = 00 (Push-Pull) for GPIO A
	  
	  GPIOA->CRL &=~ (0xF << 0 );         // clear PA0
	  GPIOA->CRL |= (0x8 << 0 );          // set PA0 as input
	  GPIOA->ODR &=~ (1 << 0 );           // set pull down

    while(1) {
			  if ( GPIOA ->IDR & (1<<0)){
			  GPIOC->BSRR = (1 << 13); 
			  GPIOA->ODR = (1 << 7); 
        delay(1000); 

			  GPIOC->BRR = (1 << 13);  
		  	GPIOA->BRR |= (1 << 7); 
        delay(1000);
				}
    }
}
