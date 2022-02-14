/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "Driver_SLCD.h" 
#include "string.h"
#include "stdio.h"

#define LDVAL_TRIGGER_1MS 20000

extern short LPTMR_INTERRUPT;
extern int int_counter;
extern void lptmr_interrupt();
extern void time_delay();
void configure_PIT(void);
extern void initPIT();
void pit_isr1(void);
void pit_interrupt_config (void);
extern void enable_irq(int);

int counter_1ms = 0;
extern int minutes;
extern int hours;

int main(void)
{
	
	STRING str = "KAIXO";

	printf("String %s\n", str);
	
	pit_interrupt_config ();
 	
	_SLCDModule_Init();
	_SLCDModule_PrintString(str, 0);
	
	char str_number[100];

	printf("\nBefore delay int_counter: %d\n", int_counter);
	
	lptmr_interrupt();
	time_delay();
	printf("After delay int_counter: %d\n", int_counter);
	
	_SLCDModule_ClearLCD(0);
	
	STRING str_seg = "SEG";
	_SLCDModule_PrintString(str_seg, 18);
	
	for(;;) {	   
	   	if(LPTMR_INTERRUPT == 1){
	   	 	LPTMR_INTERRUPT = 0; 

	   	 	sprintf(str_number, "%d", int_counter);
	   	 	_SLCDModule_PrintString(str_number, 0);
	   	 	sprintf(str_number, "%d", minutes);
	   	 	_SLCDModule_PrintString(str_number, 12);
	   	 	sprintf(str_number, "%d", hours);
	   	 	_SLCDModule_PrintString(str_number, 24);
	   	 	//printf("counter_1ms: %d\n", counter_1ms);
	   	 	//printf("int_counter: %d\n", int_counter);
	   	}
	}
	return 0;
}

// Cada interrupcion del PIT1 se activa y desactiva el PORTA.8 (250us/750us)
void pit_isr1(void)
{
    PIT_TFLG0 |= PIT_TFLG_TIF_MASK;

	counter_1ms++;
	if(counter_1ms == 999){
		//printf("counter_1ms: %d\n", counter_1ms);
		//printf("int_counter: %d\n", int_counter);
		counter_1ms = 0;
	}
}



void pit_interrupt_config (void)
 {
    // Enable the clock to the PIT module
    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
    PIT_MCR = PIT_MCR_FRZ_MASK;
    PIT_TCTRL0 = 0;
    PIT_LDVAL0 = LDVAL_TRIGGER_1MS; // Trigger 1 ms
    PIT_TFLG0 |= PIT_TFLG_TIF_MASK;
    enable_irq(68);
    PIT_TCTRL0 = (PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK);
    
    NVICIP17 = 1;
}
