#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

void handleInterrupt();

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  	

	*GPIO_PA_DOUT = 0x0000;
	
	if (*DAC0_CH0DATA){
		*DAC0_CH0DATA = 0;
		*DAC0_CH0DATA = 0;
		
	}
	else {
		*DAC0_CH0DATA = 0xff;
		*DAC0_CH0DATA = 0xff;
		

	}
	*TIMER1_IFC = 1;
	

/*	

    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
 	4. 	Write a continuous stream of samples to the DAC data registers, DAC0_CH0DATA and 			DAC0_CH1DATA, for example from a timer interrupt
  */  
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
   	*GPIO_IFC = 0xff;
	handleInterrupt();
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{	
	*GPIO_IFC = 0xff;
    	handleInterrupt();
}

void handleInterrupt() {
	
	*GPIO_PA_DOUT = *GPIO_PC_DIN << 8;
	

}


