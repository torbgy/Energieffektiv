#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "math.h"

void handleInterrupt();


uint16_t counter;
uint16_t soundPeriod;
uint16_t b = 0;
uint16_t mode = 0;
uint16_t array[3][40] = 
{
{0,10,20,30,40,50,60,70,80,90,100,90,80,70,60,50,40,30,20,15,20,10,0,10,20,10,0,20,40,60,80,90,100,90,80,60,40,30,20,10}, {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,95,90,85,80,75,70,65,60,55,50,45,40,35,30,25,20,15,10,5}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100}
};

uint16_t freq[9] = { 261, 293,329,349,392,440,493,523,600};
const static int ofreq = 14000000;
/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  	
	*TIMER1_IFC = 1;
	counter++;
	
	if (counter == 40){
		counter = 0;
	}

	*DAC0_CH0DATA = (0x5ff/100)*array[mode][counter];
	*DAC0_CH1DATA = (0x5ff/100)*array[mode][counter];
		

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
				*TIMER1_TOP = ofreq/(40*freq[0]);
				break;
			case (0xfffd) : 
				*TIMER1_TOP = ofreq/(40*freq[1]);
				break;
			case (0xfffb) : 
				*TIMER1_TOP = ofreq/(40*freq[2]);
				break;
			case (0xfff7) : 
				*TIMER1_TOP = ofreq/(40*freq[3]);
				break;				
			case (0xffef) :
				*TIMER1_TOP = ofreq/(40*freq[4]);
				break; 
			case (0xffdf) : 
				*TIMER1_TOP = ofreq/(40*freq[5]);
				break;
			case (0xffbf) : 
				*TIMER1_TOP = ofreq/(40*freq[6]);
				break;
			case (0xff7f) : 
				*TIMER1_TOP = ofreq/(40*freq[7]);
				break;
			default : 
				*TIMER1_TOP = ofreq/(40*freq[8]);
				mode++;
				if ( mode > 2){
					mode = 0;
				}
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


