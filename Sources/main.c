/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "Driver_SLCD.h" 
#include "string.h"
#include "stdio.h"

#define FTM1_MOD_VALUE 24000 //Periodo T (0-65536)

void _PWM(float DutyCycle);
void initPWM();

extern short LPTMR_INTERRUPT;
extern int int_counter;
extern void lptmr_interrupt();
extern void time_delay();
void configure_PIT(void);
void disable_watchdog(void);
extern void initPIT();

void printLCDString(char *str);

int main(void)
{
	
	STRING str = "KAIXO";

	printf("String %s\n", str);

	//disable_watchdog();

	//configure_PIT();
	
	initPIT();
 	
	//initPWM();
	//_PWM(0.6);

	// _SLCDModule_Init();
	// _SLCDModule_PrintString(str, 0);
	
	// char str_number[100];

	// printf("\nbefore delay int_counter: %d\n", int_counter);
	
	// lptmr_interrupt();
	// time_delay();
	// printf("after delay int_counter: %d\n", int_counter);
	
	// _SLCDModule_ClearLCD(0);
	
	// STRING str_seg = "SEG";
	// _SLCDModule_PrintString(str_seg, 18);
	
	// int_counter = 0;
	
	for(;;) {	   
	   	// if(LPTMR_INTERRUPT == 1){
	   	// 	LPTMR_INTERRUPT = 0;  		
	   	// 	sprintf(str_number, "%d", int_counter);
	   	// 	_SLCDModule_PrintString(str_number, 0);
	   		
	   	// 	printf("counter: %d\n", int_counter);
	   	// }
	}
	return 0;
}


void initPWM(){
	 //Habilitamos el clock al FTM1 (Flex Timer Module)
	 SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;
	 // Configuraci�n puerto PTA8 (buzzer) como salida:
	 PORTA_PCR8 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
	 // Configuracion Timer
	 FTM1_MODE |= FTM_MODE_WPDIS_MASK;
	 FTM1_MODE &= ~1;
	 FTM1_QDCTRL &=~FTM_QDCTRL_QUADEN_MASK;
	 FTM1_CNT = 0x0;
	 FTM1_MOD = FTM1_MOD_VALUE;
	 FTM1_CNTIN = 0;
	 FTM1_C0SC |= FTM_CnSC_ELSB_MASK;
	 FTM1_C0SC &= ~FTM_CnSC_ELSA_MASK;
	 FTM1_C0SC |= FTM_CnSC_MSB_MASK;
	 FTM1_C0V = FTM1_MOD_VALUE;
	 FTM1_SC = FTM_SC_PS(0) | FTM_SC_CLKS(1);
}

void _PWM(float DutyCycle){
	float D = DutyCycle*0.01;//D en tanto por 1
	//Toff=(1-D)*T
	FTM1_C0V = (int) ((1.0-D) * (float)FTM1_MOD_VALUE);
}


void disable_watchdog()
{
	asm(" CPSID i");
	/* Write 0xC520 to the unlock register */
	WDOG_UNLOCK = 0xC520;
	/* Followed by 0xD928 to complete the unlock */
	WDOG_UNLOCK = 0xD928;
	/* enable all interrupts */
	asm(" CPSIE i");
	/* Clear the WDOGEN bit to disable the watchdog */
	WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

void configure_PIT(){
	// Enable clock for port A
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;

	//Set PTA10 (connected to LED's) for GPIO functionality
	PORTA_PCR10 = (0|PORT_PCR_MUX(1));


	//Change PTA10 in output mode
	//GPIOA_PDDR = GPIO_PDDR_PDD(GPIO_PIN(10));

	//Enable external clock (50mhz)
	OSC_CR |= OSC_CR_ERCLKEN_MASK;


	//Select system oscillator for MCGCLKSEL
	SIM_SOPT2 &= ~SIM_SOPT2_MCGCLKSEL_MASK;



	//
	MCG_C1 |= MCG_C1_CLKS(0x2);

	//Divide frequency by 10 on OUTDIV2 (bus clock)
	SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV2(0xA);

	//*************************** PIT *************************************

	//Activate PIT clock in SIM_SCGC6
	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

	//Turn on PIT timer

	PIT_MCR &= ~PIT_MCR_MDIS_MASK;



	//Timer will stop in debug mode
	PIT_MCR |= PIT_MCR_FRZ_MASK;


	// Timer 2
	PIT_LDVAL2 = 0x26259F; //2 500 000 - 1


	// TIF = Time Interrupt flag. Is cleared by writing it with 1.
	PIT_TFLG2 = PIT_TFLG_TIF_MASK;


	NVICIP70 = 0x30; //Set priority 3 
	NVICICPR2|=(1<<6);//Clear any pending interrupts
	NVICISER2|=(1<<6);//Enable interrupts


	PIT_TCTRL2 = PIT_TCTRL_TIE_MASK;
	PIT_TCTRL2 |= PIT_TCTRL_TEN_MASK;
}
