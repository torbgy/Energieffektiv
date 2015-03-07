#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"


void setupDAC()
{
	*CMU_HFPERCLKEN0 |= 0x20000;
	*DAC0_CTRL = 0x50010;

}


