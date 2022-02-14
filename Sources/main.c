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
extern void lptmr_interrupt();
extern void time_delay();
void configure_PIT(void);
extern void initPIT();
void pit_isr1(void);
void pit_interrupt_config (void);
extern void enable_irq(int);

int counter_1ms = 0;
extern int seconds;
extern int minutes;
extern int hours;

int main(void)
{
	
	STRING str = "KAIXO";

	printf("String %s\n", str);
	
	pit_interrupt_config ();
 	
	_SLCDModule_Init();
	_SLCDModule_PrintString(str, 0);
	
	char str_seconds[10];
	char str_minutes[10];
	char str_hours[10];
	char str_time[50];

	printf("\nBefore delay seconds: %d\n", seconds);
	
	lptmr_interrupt();
	time_delay();
	printf("After delay seconds: %d\n", seconds);
	
	_SLCDModule_ClearLCD(0);
	
	for(;;) {	   
	   	if(LPTMR_INTERRUPT == 1){
	   	 	LPTMR_INTERRUPT = 0; 

	   	 	if(seconds >= 10) {
				sprintf(str_seconds, "%d", seconds);
			}
			else{
				sprintf(str_seconds, "0%d", seconds);
			}
	   	 	if(minutes >= 10) {
				sprintf(str_minutes, "%d", minutes);
			}
			else{
				sprintf(str_minutes, "0%d", minutes);
			}
	   	 	if(hours >= 10) {
				sprintf(str_hours, "%d", hours);
			}
			else{
				sprintf(str_hours, "0%d", hours);
			}
			sprintf(str_time, "%s%s%s", str_hours, str_minutes, str_seconds);
	   	 	_SLCDModule_PrintString(str_time, 0);
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
		//printf("seconds: %d\n", seconds);
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


void initPWM(){
	 //Habilitamos el clock al FTM1 (Flex Timer Module)
	 SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;
	 // Configuración puerto PTA8 (buzzer) como salida:
	 PORTA_PCR8 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
	 // Configuracion Timer
	 FTM1_MODE |= FTM_MODE_WPDIS_MASK;
	 FTM1_MODE &= ~1;
	 FTM1_QDCTRL &=~FTM_QDCTRL_QUADEN_MASK;
	 FTM1_CNT = 0x0;
	 //FTM1_MOD = FTM1_MOD_VALUE;
	 FTM1_CNTIN = 0;
	 FTM1_C0SC |= FTM_CnSC_ELSB_MASK;
	 FTM1_C0SC &= ~FTM_CnSC_ELSA_MASK;
	 FTM1_C0SC |= FTM_CnSC_MSB_MASK;
	 //FTM1_C0V = FTM1_MOD_VALUE;
	 FTM1_SC = FTM_SC_PS(0) | FTM_SC_CLKS(1);
}
void _PWM(float DutyCycle){
	float D = DutyCycle*0.01;//D en tanto por 1
	//Toff=(1-D)*T
	//FTM1_C0V = (int) ((1.0-D) * (float)FTM1_MOD_VALUE);
}
