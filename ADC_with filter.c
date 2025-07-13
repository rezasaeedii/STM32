#include "stm32f10x.h"
#include <math.h>
// define global parameters //
volatile int counter = 0;
float output_data = 0;
float real_data = 0;


// define functions //
void TIM2_IRQHandler (void);
void TimeInit (void);
void ADCInit (void);
float setoutput (float data);
  

// main project //
int main(void) {
	
	  
	
	  //----------------------------------------------------------// set clock
	
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;           // set clock for GPIO A
	  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;           // set clock for GPIO B
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;           // set clock for GPIO C
	  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; 					// set clock for AFIO
	  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;           // set clock for TIM2
	  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; 					// set clock for ADC
  	RCC->CFGR    |= RCC_CFGR_ADCPRE_DIV4;         // divide clock : 4

	
	  //----------------------------------------------------------// set GPIO
	  
	  GPIOB->CRL &= ~(0xF << 4) ;                   // reset PB1
	
	  GPIOB->CRL |= (0x0 << 4);                     // MODE0 = 00 , CNF0 = 00  set as input Analog
	
	  GPIOA->CRL &= ~(0xF << 8 );                   // reset PA2
	  GPIOA->CRL |=  (0x8  << 8 );                  // MODE0 = 00 , CNF0 = 10  set as input
	  GPIOA->ODR &= ~(1 << 2 );
	  
	  GPIOC->CRH &= ~(0xF << 20 );                  // reset PC13
	  GPIOC->CRH |=  (0x3 << 20 );                  // MODE0 = 11 , CNF0 = 00  set as output
	
	  
		//----------------------------------------------------------//
	
	  TimeInit();
	  ADCInit();
	  
    //----------------------------------------------------------//
	
    while(1) {
			
			 if (counter >= 50){ 
					ADC1->CR2 |= ADC_CR2_SWSTART;                 // start calibration
					while((ADC1->SR & ADC_SR_EOC)== (0 << 1));    // EOC = End of calibration
					
					output_data = (ADC1->DR * 3.3) / 4095;
				 
					real_data = setoutput(output_data);           // check data for valid
				  real_data = (int)(real_data * 100) / 100.0f;
					
				 
			 ADC1->CR2 &= ~(ADC_CR2_SWSTART);
   
			 
			 counter = 0;
			 }	
    }
}



// define functions //

void TIM2_IRQHandler (void){ 
		if(TIM2->SR & TIM_SR_UIF){                      // SR is a register for check status of intrupt
			  counter ++ ;
			 TIM2->SR &= ~TIM_SR_UIF;                     // reset status to default
		}
}

void TimeInit (void){
	
	
	  TIM2->PSC   = 7200 - 1;                       // set clock 10KHz
	  TIM2->ARR   = 10 - 1;                         // timescale = 1ms
	  TIM2->RCR   = 0;
	  TIM2->CR1  |= TIM_CR1_CEN;                    // counter enable
    TIM2->DIER |= TIM_DIER_UIE;                   // enable intrupt
	
	  NVIC_EnableIRQ(TIM2_IRQn);                    // set function for all part       

}

void ADCInit (void){
	  
	  ADC1->SQR3 = 9 ;                              // set chanel 9 for out ADC
	  ADC1->SMPR2 = 5 << 27;                        // sample for 55.5 cycle
	
	  ADC1->CR2 |= ADC_CR2_ADON |                   // set bit 0 : converter ON/OFF
                 ADC_CR2_RSTCAL |                 // set bit 3 : REset Calibration
                 ADC_CR2_EXTSEL |                 // set bit 17,18,19 : : External event select for regular group
	               ADC_CR2_EXTTRIG;                 // set bit 20 : External trigger conversion mode for regular channels
	  
	  while(ADC1->CR2 & ADC_CR2_RSTCAL);            // wait for end of calibration
	  
	  ADC1->CR2 |= ADC_CR2_CAL;                     // start calibration
	  while(ADC1->CR2 & ADC_CR2_CAL);               // wait for calibration

}


float setoutput(float data) {
    static float base_data = 0;
    static float data_line[10] = {0}; 
    static int index = 0;
    static int buffer_full = 0;

    data_line[index] = data;                     // set data in buffer
    index = (index + 1) % 10;                    // get correct index

    if (!buffer_full && index == 0) 
        buffer_full = 1;                         // check empety

    if (buffer_full) {                           // if our buffer is full now we can calculate the output
        float sum = 0, avg;
        int correct = 1;

        for (int i = 0; i < 10; i++) 
            sum += data_line[i];
        avg = sum / 10.0f;

        
        for (int i = 0; i < 10; i++) {
            if (fabsf(data_line[i] - avg) > 0.2f) { // check for trashold 0.2
                correct = 0;
                break;
            }
        }

        if (correct) 
            return base_data = avg;
    }
    
    return 0.00;
}
