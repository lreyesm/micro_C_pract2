/*
 *    kinetis_sysinit.c - Default init routines for P2
 *                     		Kinetis ARM systems
 *    Copyright � 2012 Freescale semiConductor Inc. All Rights Reserved.
 */
 
#include "kinetis_sysinit.h"
#include "derivative.h"
#include "stdio.h"

#define LDVAL_TRIGGER_1MS 20000
#define LDVAL_TRIGGER_250US 5000
/**
 **===========================================================================
 **  External declarations
 **===========================================================================
 */
#if __cplusplus
extern "C" {
#endif
extern uint32_t __vector_table[];
extern unsigned long _estack;
extern void __thumb_startup(void);
#if __cplusplus
}
#endif

/**
 **===========================================================================
 **  User functions
 **===========================================================================
 */
void enable_irq (int irq);
void time_delay(unsigned int delay);
void lptmr_time_counter();
void lptmr_clear_registers();
void lptmr_interrupt();
void lptmr_isr(void);
void initPIT (void);
void pit_interrupt_config (void);
void pit_isr1(void);
void pit_isr2(void);
void isr_PIT2(void);

/**
 **===========================================================================
 **  Default interrupt handler
 **===========================================================================
 */
void Default_Handler()
{
	__asm("bkpt");
}

/**
 **===========================================================================
 **  Reset handler
 **===========================================================================
 */
void __init_hardware()
{
	SCB_VTOR = (uint32_t)__vector_table; /* Set the interrupt vector table position */
	/*
		Disable the Watchdog because it may reset the core before entering main().
		There are 2 unlock words which shall be provided in sequence before
		accessing the control register.
	*/
	WDOG_UNLOCK = KINETIS_WDOG_UNLOCK_SEQ_1;
	WDOG_UNLOCK = KINETIS_WDOG_UNLOCK_SEQ_2;
	WDOG_STCTRLH = KINETIS_WDOG_DISABLED_CTRL;
}

/* Weak definitions of handlers point to Default_Handler if not implemented */
void NMI_Handler() __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler() __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler() __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler() __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler() __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler() __attribute__ ((weak, alias("Default_Handler")));
void DebugMonitor_Handler() __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler() __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler() __attribute__ ((weak, alias("Default_Handler")));


/* The Interrupt Vector Table */
void (* const InterruptVector[])() __attribute__ ((section(".vectortable"))) = {
    /* Processor exceptions */
    (void(*)(void)) &_estack,
    __thumb_startup,
    NMI_Handler, 
    HardFault_Handler, 
    MemManage_Handler, 
    BusFault_Handler,
    UsageFault_Handler, 
    0, 
    0, 
    0, 
    0, 
    SVC_Handler, 
    DebugMonitor_Handler, 
    0,
    PendSV_Handler, 
    SysTick_Handler,

    /* Interrupts */
    Default_Handler, //16
    Default_Handler, //17
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler, //67
    pit_isr1, //68
    pit_isr1, //69
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler, //84
    lptmr_isr,   //85
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,	
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,	
    Default_Handler,
    Default_Handler,
    Default_Handler,		
};

void enable_irq (int irq)
{
    int div;
    
    /* Make sure that the IRQ is an allowable number. Right now up to 91 is 
     * used.
     */
    if (irq > 91)
        printf("\nERR! Invalid IRQ value passed to enable irq function!\n");
    
    /* Determine which of the NVICISERs corresponds to the irq */
    div = irq/32;
    
    switch (div)
    {
    	case 0x0:
              NVICICPR0 |= 1 << (irq%32);
              NVICISER0 |= 1 << (irq%32);
              break;
    	case 0x1:
              NVICICPR1 |= 1 << (irq%32);
              NVICISER1 |= 1 << (irq%32);
              break;
    	case 0x2:
              NVICICPR2 |= 1 << (irq%32);
              NVICISER2 |= 1 << (irq%32);
              break;
    }              
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : time_delay 
* Returned Value   :
* Comments         : Wait until interrupt of timer occur
*    
*
*END*----------------------------------------------------------------------*/
void time_delay(unsigned int delay) 
{
	unsigned int u32Delay;
	unsigned int u32Delay2;
	unsigned int u32Delay3;
   
	for(u32Delay=0;u32Delay<delay;u32Delay++);
	  for(u32Delay2=0;u32Delay2<16;u32Delay2++)
		for(u32Delay3=0;u32Delay3<0x35551;u32Delay3++);
}

/*
 * Delay function using the LPTMR module
 */
void lptmr_time_counter()
{
    int compare_value=1000;  //value must be less than 0xFFFF or 65535

    //Enable Port clocks
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

    /* Enable LPT Module */
    SIM_SCGC5|=SIM_SCGC5_LPTIMER_MASK;

    lptmr_clear_registers();

    /* Configure LPTMR */
    LPTMR0_CMR=LPTMR_CMR_COMPARE(compare_value);  //Set compare value
    LPTMR0_PSR=LPTMR_PSR_PCS(0x1)|LPTMR_PSR_PBYP_MASK;  //Use LPO clock and bypass prescale

    LPTMR0_CSR|=LPTMR_CSR_TEN_MASK; //Turn on LPTMR with default settings

    //Wait for Timer Compare Flag to be set
    while((LPTMR0_CSR & LPTMR_CSR_TCF_MASK)==0)
    {
      //This may not get proper counter data if the CNR is read at the same time it is incremented
    // printf("Current value of counter register CNR is %d\n",LPTMR0_CNR);
    }
    
    LPTMR0_CSR|=LPTMR_CSR_TCF_MASK; 
  
}

void lptmr_clear_registers()
{
  LPTMR0_CSR=0x00;
  LPTMR0_PSR=0x00;
  LPTMR0_CMR=0x00;
}

/*
 * Timer will trigger interrupt 
 */
void lptmr_interrupt()
{
  int compare_value=1000;  //value must be less than 0xFFFF

  //Enable Port clocks
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

  /* Enable LPT Module */
  SIM_SCGC5|=SIM_SCGC5_LPTIMER_MASK;

  LPTMR_INTERRUPT=0; //Clear global variable

  //Reset LPTMR module
  lptmr_clear_registers();

  /* Enable LPT Interrupt in NVIC*/
  enable_irq(85); //LPTMR Vector is 101. IRQ# is 101-16=85

  /* Configure LPT */
  LPTMR0_CMR=LPTMR_CMR_COMPARE(compare_value);  //Set compare value
  LPTMR0_PSR=LPTMR_PSR_PCS(0x1)|LPTMR_PSR_PBYP_MASK;  //Use LPO clock and bypass prescale
  LPTMR0_CSR=LPTMR_CSR_TIE_MASK;  //Enable LPT interrupt

  LPTMR0_CSR|=LPTMR_CSR_TEN_MASK; //Turn on LPTMR and start counting

}

void lptmr_isr(void)
{
  LPTMR0_CSR|=LPTMR_CSR_TCF_MASK;  //Clear LPT Compare flag
  LPTMR_INTERRUPT=1;  //Set global variable
  int_counter++;
  // pasa_segundo = 1;
 // printf("\n\nIn LPT ISR!\n\n");
 // printf("Veces en interrupcion %d \n", int_counter);
}

 
void initPIT (void){
  // Configura el pin 8 del puerto A como GGIO
  // Inicializa pin 8 de PORT A como GPIO para buzzer
  PORT_PCR_REG(PORTA_BASE_PTR, 8) = PORT_PCR_MUX(1);
  GPIOA_PDDR |= (1ul << 8);
  // Ejemplo si F=20MHz (pulso 50ns), Configura PIT0 con 1ms y PIT1 con 250us
  pit_interrupt_config();
 
}

void pit_interrupt_config (void)
 {
    // Enable the clock to the PIT module
    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
    PIT_MCR = PIT_MCR_FRZ_MASK;
    PIT_TCTRL0 = 0;
    PIT_TCTRL1 = 0;
    PIT_LDVAL0 = LDVAL_TRIGGER_1MS; // Trigger 1 ms
    PIT_LDVAL1 = LDVAL_TRIGGER_250US; // Trigger 250 us
    PIT_TFLG0 |= PIT_TFLG_TIF_MASK;
    PIT_TFLG1 |= PIT_TFLG_TIF_MASK;
    enable_irq(68);
    enable_irq(69);
    PIT_TCTRL0 = (PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK);
    PIT_TCTRL1 = (PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK);
}
// Cada interrupci�n del PIT1 se activa y desactiva el PORTA.8 (250us/750us)
void pit_isr1(void)
{
    volatile static unsigned char count = 0;
    PIT_TFLG1 |= PIT_TFLG_TIF_MASK;
    PIT_LDVAL1 = LDVAL_TRIGGER_250US; // 250uS

    if (count < 1)
    {
    	GPIOA_PSOR |= (1ul << 8);
    	count++;
    }
    else
    {
    	GPIOA_PCOR |= (1ul << 8);
    	count++;
		if (count >= 4)
		{
			count = 0;
		}
    }
	printf("Interrupcion");
}
void pit_isr2(void){
	printf("Interrupcion");
}



 

void isr_PIT2(void)
{
	PIT_TFLG2 = 0x01;
	
	//Toggle blue LED on each interrupt
	printf("Interrupcion PIT");
}


