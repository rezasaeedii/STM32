#include "system.h"

int HSE_ERROR = 0;

void X_init(void){  // init serial wire //
	
		RCC->APB2ENR  	 |=  RCC_APB2ENR_AFIOEN;  	  // set AFIO clock //
	  AFIO->MAPR   	   &= ~AFIO_MAPR_SWJ_CFG;     
	  AFIO->MAPR    	 |=  AFIO_MAPR_SWJ_CFG_1; 	  // disable JTAG and enable Serial wire //
		
}


void X_Clock(void){  // clock cofiguration //
	
		volatile int HSEcounter = 0;
		RCC->CR       	 &= ~RCC_CR_HSEBYP;       	  // set cristal clock 8M Hz //
		RCC->CR          |=  RCC_CR_HSEON;        	  // enable HSE //
	  do HSEcounter++;                	            // wait for ready HSE , if take long time just pass //
	  while(((RCC->CR) & (RCC_CR_HSERDY) == 0x0) && (HSEcounter <= 0x500)); 
		
		if((RCC->CR) & (RCC_CR_HSERDY) != 0x0){       // check that cristal is works //
				
				FLASH->ACR     |=  FLASH_ACR_PRFTBE;      // enable Prefetch //
				FLASH->ACR     &= ~FLASH_ACR_LATENCY; 
				FLASH->ACR     |=  FLASH_ACR_LATENCY_1;   // set LATENCY = 010 that mean CPU have to wait for 2 clock cycle or 48MHz < SYSCLK < 72MHz//
				
				RCC->CFGR      &= ~RCC_CFGR_HPRE;         // HPRE = /1 //
				
				RCC->CFGR      &= ~RCC_CFGR_PPRE1; 
				RCC->CFGR      |=  RCC_CFGR_PPRE1_2;      // PPRE1 = /2 //
				
				RCC->CFGR      &= ~RCC_CFGR_PPRE2;        // PPRE1 = /1 //

				RCC->CFGR      &= ~RCC_CFGR_ADCPRE;       
				RCC->CFGR      |=  RCC_CFGR_ADCPRE_1;     // ADCPRE = /6 //
				 
				RCC->CFGR      &= ~RCC_CFGR_USBPRE;       // USBPRE = /1.5 //
				
				RCC->CFGR      &= ~RCC_CFGR_PLLXTPRE;     // PLLXTPRE = /1 //
				
				RCC->CFGR      &= ~RCC_CFGR_PLLSRC;       // PLL SOURSE MUX = HSI /2 //
				
				RCC->CFGR      &= ~RCC_CFGR_PLLMULL;      
				RCC->CFGR      |=  RCC_CFGR_PLLMULL9;     // PLLMULL = *9 //
				
				RCC->CR        |=  RCC_CR_PLLON;          // enable PLL //
				while(((RCC->CR) & (RCC_CR_PLLRDY)) == 0x0); 
				
				RCC->CFGR      &= ~RCC_CFGR_SW;
				RCC->CFGR      |=  RCC_CFGR_SW_PLL;       // SYSTEM CLOCK MUX = PLL , SWS for check output mux//
				
				while(((RCC->CFGR) & (RCC_CFGR_SWS)) != 0x8); 
			
		}
    else HSE_ERROR = 1;
}