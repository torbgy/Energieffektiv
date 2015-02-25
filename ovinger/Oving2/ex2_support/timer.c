#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"
//int startupArr[ = { e329 _ e _ e _ c261 _ _ e _ _ g392 _ _ _ _ 196lowG}
/* function to setup the timer */
void setupTimer(){
  /*
    TODO enable and set up the timer
    
    1. Enable clock to timer by setting bit 6 in CMU_HFPERCLKEN0
    2. Write the period to register TIMER1_TOP
    3. Enable timer interrupt generation by writing 1 to TIMER1_IEN
    4. Start the timer by writing 1 to TIMER1_CMD
    
    This will cause a timer interrupt to be generated every (period) cycles. Remember to configure the NVIC as well, otherwise the interrupt handler will not be invoked.
  */  

	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_TIMER1;
	int a = 0;
	*TIMER1_TOP = 14000000/(20*200);
	*TIMER1_IEN = 1;
	*TIMER1_CMD = 1;	
	while (a<140000){
		a++;
		}
	a = 0;
	
	*TIMER1_IEN = 0;
	*TIMER1_CMD = 0;	
	while (a<140000){
		a++;
		}
	a = 0;
	*TIMER1_TOP = 14000000/(20*440);
	*TIMER1_IEN = 1;
	*TIMER1_CMD = 1;
	while (a<140000){
		a++;
		}
	a = 0;
	*TIMER1_IEN = 0;
	*TIMER1_CMD = 0;
}

