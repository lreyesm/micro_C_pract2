/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "Driver_SLCD.h" 
#include "string.h"
#include "stdio.h"

#define LDVAL_TRIGGER_1MS 20000

void pit_isr1				(void);
void lptmr_isr				(void);
void _PWM					(float);
void initPWM				(void);
void initPIT				(void);
void enable_irq				(int);
void lptmr_interrupt		(void);
void lptmr_time_counter		(void);
void lptmr_clear_registers	(void);
void time_delay				(unsigned int);
void pit_interrupt_config	(void);
void printTimeInLCD			(void);


short LPTMR_INTERRUPT =0;
int seconds = 0;
int minutes = 0;
int hours = 0;

int counter_1ms = 0;
int array_ms[100];


int main(void)
{
	
	STRING str = "KAIXO";

	printf("String %s\n", str);
	
	pit_interrupt_config ();

	_SLCDModule_Init();
	_SLCDModule_PrintString(str, 0);
	


	printf("\nBefore delay seconds: %d\n", seconds);
	
	lptmr_interrupt();
	time_delay(3);
	printf("After delay seconds: %d\n", seconds);
	
	_SLCDModule_ClearLCD(0);
	
	for(;;) {	   
		if(LPTMR_INTERRUPT == 1){
			LPTMR_INTERRUPT = 0;		
			array_ms[seconds] = counter_1ms;
			printTimeInLCD();
		}
	}
	return 0;
}

void printTimeInLCD(){
	char str_seconds[10];
	char str_minutes[10];
	char str_hours[10];
	char str_time[50];
	
	if(seconds >= 60) {
		seconds = 0;
		minutes++;
		if(minutes >= 60) {
			minutes = 0;
			hours++;
			if(hours >= 24) hours = 0;
		}
	}
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

// Cada interrupcion del PIT1 se activa y desactiva el PORTA.8 (250us/750us)
void pit_isr1(void)
{
    PIT_TFLG0 |= PIT_TFLG_TIF_MASK;
	counter_1ms++;
	if(counter_1ms >= 999){
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


void lptmr_isr(void)
{
  	LPTMR0_CSR|=LPTMR_CSR_TCF_MASK;  //Clear LPT Compare flag
  	LPTMR_INTERRUPT=1;  //Set global variable
	seconds++;
}


void initPIT (void){
	// Configura el pin 8 del puerto A como GGIO
	// Inicializa pin 8 de PORT A como GPIO para buzzer
	PORT_PCR_REG(PORTA_BASE_PTR, 8) = PORT_PCR_MUX(1);
	GPIOA_PDDR |= (1ul << 8);
	// Ejemplo si F=20MHz (pulso 50ns), Configura PIT0 con 1ms y PIT1 con 250us
	pit_interrupt_config();
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
