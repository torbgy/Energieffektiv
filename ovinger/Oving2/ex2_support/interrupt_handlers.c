#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "math.h"

void handleInterrupt();
void DACandTIMER(uint16_t on);


uint16_t counter;
uint16_t songCounter=0;
uint16_t b = 0;
uint16_t mode = 0;
uint16_t array[3][40] = {
{0,10,20,30,40,50,60,70,80,90,100,90,80,70,60,50,40,30,20,15,20,10,0,10,20,10,0,20,40,60,80,90,100,90,80,60,40,30,20,10}, {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,95,90,85,80,75,70,65,60,55,50,45,40,35,30,25,20,15,10,5}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80}
};
uint16_t song[12] = {329,329,0,329,0,261,329,0,392,0,0,196};
uint16_t freq[9] = { 261,293,329,349,392,440,493,523,600 };
const static int ofreq = 14000000;


void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  	
	*TIMER1_IFC = 1;
	counter++;
	if (counter == 40){
		counter = 0;
	}
	
	*DAC0_CH0DATA = (0x5ff/100)*array[mode][counter];
	*DAC0_CH1DATA = (0x5ff/100)*array[mode][counter];	
	
}

void __attribute__ ((interrupt)) LETIMER0_IRQHandler()
{
	
	*LETIMER0_IFC = *LETIMER0_IF;
	if (songCounter < 12){
		if(song[songCounter] != 0){
			*TIMER1_TOP = ofreq/(40*song[songCounter]);
			DACandTIMER(1);
			*SCR = 0x2; 
		}else{
			DACandTIMER(0);
			*SCR = 0x6;
		}
	}else{
		songCounter = 0;
		*LETIMER0_CMD = 0; 
		*LETIMER0_IEN = 0;
		DACandTIMER(0);
		*SCR = 0x6;
	}
 	songCounter++;
}

void playSong(){
	
	*SCR = 0x2;
	*LETIMER0_CMD = 1;
	*LETIMER0_IEN = 1;
}

void handleInterrupt() {
	
	b = *GPIO_PC_DIN | 0xff00;
	*GPIO_PA_DOUT = b << 8;
	
	*SCR = 0x2;
	
	switch (b){
		case (0xfffe) : 
			//*TIMER1_TOP = ofreq/(40*freq[0]);
			playSong();
			break;
		case (0xfffd) : 
			*TIMER1_TOP = ofreq/(40*freq[1]);
			DACandTIMER(1);			
			break;
		case (0xfffb) : 
			*TIMER1_TOP = ofreq/(40*freq[2]);
			DACandTIMER(1);
			break;
		case (0xfff7) : 
			*TIMER1_TOP = ofreq/(40*freq[3]);
			DACandTIMER(1);
			break;				
		case (0xffef) :
			*TIMER1_TOP = ofreq/(40*freq[4]);
			DACandTIMER(1);
			break; 
		case (0xffdf) : 
			*TIMER1_TOP = ofreq/(40*freq[5]);
			DACandTIMER(1);
			break;
		case (0xffbf) : 
			*TIMER1_TOP = ofreq/(40*freq[6]);
			DACandTIMER(1);
			break;
		case (0xff7f) : 
			*TIMER1_TOP = ofreq/(40*freq[7]);
			DACandTIMER(1);
			break;
		case (0xffff) : 
			DACandTIMER(0);
			*SCR = 0x6;
			break;
		default : 
			*TIMER1_TOP = ofreq/(40*freq[8]);
			mode++;
			if ( mode > 2){
				mode = 0;
			}
			break;
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

void DACandTIMER(uint16_t on)
{

	*DAC0_CH0CTRL = on;
	*DAC0_CH1CTRL = on;
	*TIMER1_IEN = on;
	*TIMER1_CMD = on;
	

}


