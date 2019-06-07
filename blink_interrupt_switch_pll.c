#include "km_tm4c123gh6pm.h"
#include "pll.h"

// Interrupt handler for falling edge

void GPIOF_Handler()
{
		GPIO_PORTF_DATA_R ^= ~(0x01); // Switch off all LEDs
		GPIO_PORTF_ICR_R |= (0xFF);
}

void PORT_PF_INIT(void)
{
		SYSCTL_RCGCGPIO_R |= 0x20; // enabling clock pulses to RCGCGPIO
		
		while(!(SYSCTL_PRGPIO_R & 0x20)) //Checking if the register is all set to take in the clock pulses
		{
			;
		}
		GPIO_PORTF_DIR_R |=0x02; // PF1 as output 
		GPIO_PORTF_DIR_R &=~(0x10); // Make switch 2 as input (PF0)
		GPIO_PORTF_PUR_R |= (0x10); // Enable the Pull down Register  
		GPIO_PORTF_DEN_R |=0x12; // PF1 to enable digital mode
		GPIO_PORTF_AMSEL_R &= ~(0x13); // Diable the analog Mode PF1 and PF0
		GPIO_PORTF_AFSEL_R &=(~0x13); // Disable alternate funcs PF1 and PF0
}

void GPIO_PORTF_INTERRUPTS_ENABLE()
{
		NVIC_EN0_R |= 0x40000000; //to enable the interrupt for port-F
		
	  GPIO_PORTF_IM_R &= ~(0x10); // mask the interrupt
	
		GPIO_PORTF_IS_R &= ~(0x10); // Edge sensitive
		GPIO_PORTF_IBE_R &= ~(0x10); // clear the control, we don't need interrupt for both rising and falling
	  GPIO_PORTF_IEV_R &= ~(0x10); // falling edge
		
   	GPIO_PORTF_IM_R |= (0x10); // UnMask the interrupt.. send the interrupt to the interrupt controller
}

void 	EnableInterrupts(void); // to enable interrupts as by defaults interrupts are off
void WaitForInterrupt(void); // to save power

int main(void)
{
		// Initialize PLL for the 
		KM_PLL_Init();
		PORT_PF_INIT();
		GPIO_PORTF_INTERRUPTS_ENABLE();
		EnableInterrupts();
		
	  while(1)
		{
				WaitForInterrupt(); // just to keep the code running
		}
}
