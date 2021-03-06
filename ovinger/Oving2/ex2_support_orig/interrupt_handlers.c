#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  

	
/*
    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
  */  
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
	*GPIO_IFC = 0xff;
	*GPIO_PA_DOUT = *GPIO_PC_DIN << 8;
    /* TODO handle button pressed event, remember to clear pending interrupt */
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
	*GPIO_IFC = 0xff;
	*GPIO_PA_DOUT = *GPIO_PC_DIN << 8;
    /* TODO handle button pressed event, remember to clear pending interrupt */
}
