#include "km_tm4c123gh6pm.h"
#include "pll.h"

void PORT_PF_INIT(void)
{
		SYSCTL_RCGCGPIO_R |= 0x20; // enabling clock pulses to RCGCGPIO
		
		while(!(SYSCTL_PRGPIO_R & 0x20)) //Checking if the register is all set to take in the clock pulses
		{
			;
		}
		GPIO_PORTF_DIR_R |=0x02; // PF1 as output 
		
		GPIO_PORTF_DEN_R |=0x02; // PF1 to enable digital mode
		GPIO_PORTF_AMSEL_R &= ~(0x02); // Diable the analog Mode PF1
		GPIO_PORTF_AFSEL_R &=(~0x02); // Disable alternate funcs PF1
}

void delay(void)
{
		int i=320000;
		while(i>=0)
			i--;
}

void SYSTICK_TIMER_INIT()
{
	// Enable Sys Clock
	 NVIC_ST_CTRL_R |= 0x04;
	
	// Disable the Enable bit
	 NVIC_ST_CTRL_R &= ~0x01;
	
	//Clear the data inside STCURRENT and COUNT flag by writing data into STCURRENT
	 NVIC_ST_CURRENT_R = 123; // Since it is a write-clear register, this earases the count flag and this register
	
	// Add the value into STRELOAD
	 NVIC_ST_RELOAD_R = 5000000;

	// Enable the SYS clock
	 NVIC_ST_CTRL_R |=0x01; 
}


int main(void)
{
	  KM_PLL_Init(); // Initialize the PLL to change the clock frequency to 50hz from 16hz
		PORT_PF_INIT(); // Initialize port settings
		SYSTICK_TIMER_INIT(); // Initialize sys-tick settings
	  
	
		
	  while(1)
		{
				GPIO_PORTF_DATA_R ^= ~(0x01); // Switch off all LEDs
				while(!(NVIC_ST_CTRL_R&0x10000))
				{
					;
				}
		}			
}
