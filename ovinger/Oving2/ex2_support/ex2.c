#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* 
  TODO 

	Fikse LETIMER0
|	EM2, men da må bruke LETIMER0 pga HFCLK ikke aktiv
	Skru av DAC for å spare energi?
	EM3?

*/




void setupTimer();
void setupDAC();
void setupNVIC();
void setupGPIO();


int main(void) 
{  
  
	setupGPIO();
	setupDAC();
	setupNVIC();
	setupTimer();

	*SCR = 2;
	
	return 0;
}

void setupNVIC()
{
	*GPIO_EXTIPSELL = 0x22222222; /* Activets interrupts on rise and fall edge for gpio*/
	*GPIO_EXTIFALL = 0xff;
	*GPIO_EXTIRISE = 0xff;
	*GPIO_IEN = 0xff;
	*ISER0 = 0x4001802; //  Enables interrupt on LETIMER0, 	TIMER1, GPIO odd, GPIO even.
		
}

