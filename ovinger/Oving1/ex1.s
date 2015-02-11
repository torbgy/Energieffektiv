        .syntax unified
	
	      .include "efm32gg.s"

	/////////////////////////////////////////////////////////////////////////////
	//
  // Exception vector table
  // This table contains addresses for all exception handlers
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .section .vectors
	
	      .long   stack_top               /* Top of Stack                 */
	      .long   _reset                  /* Reset Handler                */
	      .long   dummy_handler           /* NMI Handler                  */
	      .long   dummy_handler           /* Hard Fault Handler           */
	      .long   dummy_handler           /* MPU Fault Handler            */
	      .long   dummy_handler           /* Bus Fault Handler            */
	      .long   dummy_handler           /* Usage Fault Handler          */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* SVCall Handler               */
	      .long   dummy_handler           /* Debug Monitor Handler        */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* PendSV Handler               */
	      .long   dummy_handler           /* SysTick Handler              */

	      /* External Interrupts */
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO even handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO odd handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler

	      .section .text
	      .globl  _reset
	      .type   _reset, %function
        .thumb_func

_reset: 	
	ldr r1, =CMU_BASE		// Aktiverer klokken
	ldr r2, [r1, #CMU_HFPERCLKEN0]
	mov r3, #1
	lsl r3, r3, #CMU_HFPERCLKEN0_GPIO
	orr r2, r2, r3
	str r2, [r1, #CMU_HFPERCLKEN0]
	
	ldr r1, =GPIO_PA_BASE
	ldr r3, =GPIO_PC_BASE
	ldr r5, =GPIO_BASE

	mov r2, #0x2			// High Drive Strength
	str r2, [r1]
	ldr r2, =0x55555555		//Setter pins 8-15 som output
	str r2, [r1, #GPIO_MODEH]
	ldr r2, =0x33333333		//0-7 som input
	str r2, [r3, #GPIO_MODEL]
	mov r2, #0xff			//Aktiv lav på buttons
	str r2, [r3, #GPIO_DOUT]

	ldr r2, =0x22222222		// Aktiverer interrupt på på høy og lav kant. 
	str r2, [r5, #GPIO_EXTIPSELL]
	mov r2, #0xff
	str r2, [r5, #GPIO_EXTIFALL]
	str r2, [r5, #GPIO_EXTIRISE]
	str r2, [r5, #GPIO_IEN]
	ldr r2, =0x802
	ldr r4, =ISER0
	str r2, [r4]

	mov r2, #0x6			//Sleep state
	ldr r4, =SCR
	str r2, [r4]
	wfi				
	

        .thumb_func
gpio_handler:  
	// r1, r3, r5 er brukt til base

	ldr r2, [r5, #GPIO_IF]
	str r2, [r5, #GPIO_IFC]	

	ldr r2, [r3, #GPIO_DIN]		//Lese inn buttons
	lsl r2, r2, #8			//Shifter 8 til siden.
	str r2, [r1, #GPIO_DOUT]	//Aktiverer lysene
	
	
	bx lr  // do nothing
	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing

