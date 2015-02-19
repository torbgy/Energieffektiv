#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "math.h"

void handleInterrupt();


uint16_t counter;
uint16_t soundPeriod;
uint16_t b = 0;

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  	
	*TIMER1_IFC = 1;
	counter++;
	
	if (counter == soundPeriod) {
		counter = 0;
	}
	*DAC0_CH0DATA = counter*0xfff/soundPeriod;
	*DAC0_CH1DATA = counter*0xfff/soundPeriod;
		
	 
	
	

/*	

    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
 	4. 	Write a continuous stream of samples to the DAC data registers, DAC0_CH0DATA and 			DAC0_CH1DATA, for example from a timer interrupt
  */  
}



void handleInterrupt() {
	
	b = *GPIO_PC_DIN | 0xff00;
	*GPIO_PA_DOUT = b << 8;

	if (b != 0xffff){
		switch (b){
			case (0xfffe) : 
				soundPeriod = 20;
				break;
			case (0xfffd) : 
				soundPeriod = 40;
				break;
			case (0xfffb) : 
				soundPeriod = 100;
				break;
			case (0xfff7) : 
				soundPeriod = 150;
				break;				
			case (0xffef) :
				soundPeriod = 200;
				break; 
			case (0xffdf) : 
				soundPeriod = 350;
				break;
			case (0xffbf) : 
				soundPeriod = 400;
				break;
			case (0xff7f) : 
				soundPeriod = 450;
				break;
			default : 
				soundPeriod = 500;
				break;
			}
	
		*TIMER1_IEN = 1;
		*TIMER1_CMD = 1;
		
	}
	else { 
		*TIMER1_IEN = 0;
		*TIMER1_CMD = 0;
	}

}


void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
	*GPIO_IFC = 0xff;
	handleInterrupt();
}

void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{	
	*GPIO_IFC = 0xff;
    	handleInterrupt();
}


