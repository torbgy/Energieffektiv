#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "math.h"

void handleInterrupt();


uint16_t counter;
uint16_t soundPeriod;
uint16_t b = 0;
uint16_t array[20] = { 0 ,20 ,40 ,60 ,80, 100, 
			80, 60, 40, 20, 
			20, 0, 20, 0, 
			40, 80, 100, 80, 40, 20};
/*{ 0 ,20 ,40 ,60 ,80, 100, 
			80, 60, 40, 20, 
			0, 20, 40, 60, 
			80, 100, 80, 60, 40, 20};*/



uint16_t freq[9] = { 261, 293,329,349,392,440,493,523,100};
const static int ofreq = 14000000;
/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  	
	*TIMER1_IFC = 1;
	counter++;
	
	if (counter == 20){
		counter = 0;
	}

	*DAC0_CH0DATA = (0xfff/100)*array[counter];
	*DAC0_CH1DATA = (0xfff/100)*array[counter];
		

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
				*TIMER1_TOP = ofreq/(20*freq[0]);
				break;
			case (0xfffd) : 
				*TIMER1_TOP = ofreq/(20*freq[1]);
				break;
			case (0xfffb) : 
				*TIMER1_TOP = ofreq/(20*freq[2]);
				break;
			case (0xfff7) : 
				*TIMER1_TOP = ofreq/(20*freq[3]);
				break;				
			case (0xffef) :
				*TIMER1_TOP = ofreq/(20*freq[4]);
				break; 
			case (0xffdf) : 
				*TIMER1_TOP = ofreq/(20*freq[5]);
				break;
			case (0xffbf) : 
				*TIMER1_TOP = ofreq/(20*freq[6]);
				break;
			case (0xff7f) : 
				*TIMER1_TOP = ofreq/(20*freq[7]);
				break;
			default : 
				*TIMER1_TOP = ofreq/(20*freq[8]);
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


