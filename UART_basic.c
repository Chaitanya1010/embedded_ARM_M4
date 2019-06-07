/*#define SYSTL_RCGCPIO *((int*) (0x400FE608)) 
#define SYSTL_PRGPIO *((int*) (0x400FEA08))
#define GPIOPORT_F_DIR *((int*) (0x40004400))
#define GPIOPORT_F_DATA *((int*) (0x400043FC))
#define GPIOPORT_F_AFSEC *((int*) (0x40004420))
#define GPIOPORT_F_DEN *((int*) (0x4000451C))
#define GPIOPORT_F_AMSEL *((int*) (0x40004228))*/
// these can be added via a company-provided file

#include "km_tm4c123gh6pm.h"

// GPIOPORT_F_DIR -> GPIO_PORTF_R
// SYSTL_PRGPIO -> SYSCTL_PRGPIO_R

void PORT_PF_INIT(void)
{
		SYSCTL_RCGCGPIO_R |= 0x20;
		
		while(!(SYSCTL_PRGPIO_R & 0x20))
		{
			;
		}
		GPIO_PORTF_DIR_R |=0x0E; // PF1 to PF3 to set o/p DIR
		GPIO_PORTF_DIR_R &= ~(0x11); // PF0 and PF4 to set i/p DIR
		GPIO_PORTF_DEN_R |=0x1F; // PF0 to PF4 Digital Mode
		GPIO_PORTF_AMSEL_R &= ~(0x1F); // Diable the analog Mode PF0 to PF4
		GPIO_PORTF_AFSEL_R &=(~0x1F); // Disable alternate funcs PF0 to PF4
		// PF0 and PF4 enable Pull-up -- 
		// makes value to 1 -- OFF state for the negative switches like these
		GPIO_PORTF_PUR_R |=0x11; 
		GPIO_PORTF_DATA_R |=0x08; // Sets Green color
		
		GPIO_PORTF_LOCK_R |=0x4C4F434B; // unlock  PF0 PIN from GPIOLOCK
		GPIO_PORTF_CR_R |=0x01;
		
}

void delay(void)
{
		int i=320000;
		while(i>=0)
			i--;
}

int main(void)
{
		PORT_PF_INIT(); // Green LED On
		
	  while(1)
		{
			if((GPIO_PORTF_DATA_R&0x10)==0) // SW1 ON
			{
					GPIO_PORTF_DATA_R &= ~(0x0E); // Switch off all LEDs
					GPIO_PORTF_DATA_R |= 0x02; // Red LED
			}
			
			
			if((GPIO_PORTF_DATA_R&0x01)==0) // SW2 ON
			{
					GPIO_PORTF_DATA_R &= ~(0x0E); // Switch off all LEDs
					GPIO_PORTF_DATA_R |= 0x04; // Blue LED
			}
			
			delay();
		}
}
