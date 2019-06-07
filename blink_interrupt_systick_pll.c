#include "km_tm4c123gh6pm.h"
#include "pll.h"

// When there's an interrupt on the systick's counter, the SysTick_Handler gets the control 
		// To use this we need the interrupt bit in SYSCTL to be set
// We don't have to call this function as when an interrupt arises, this will be called automatically.
// We don't have to wait for any condition in main, we just have to keep a non-functional infinite loop to keep the code running

void SysTick_Handler()
{
		GPIO_PORTF_DATA_R ^= ~(0x01); // Switch off all LEDs
}

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

void SYSTICK_TIMER_INIT_WITH_INTERRUPT_FLAG()
{
	// Enable Sys Clock
	 NVIC_ST_CTRL_R |= 0x04;

	// Enable Sys Interrupt
	 NVIC_ST_CTRL_R |= 0x02;
	
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
		KM_PLL_Init();
		PORT_PF_INIT();
		SYSTICK_TIMER_INIT_WITH_INTERRUPT_FLAG();
		
	  while(1)
		{
				; // just to keep the code running
		}
}
