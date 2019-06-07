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

void PORT_PA_INIT(void)
{
		SYSCTL_RCGCGPIO_R |= 0x01;
		
		while(!(SYSCTL_PRGPIO_R & 0x01))
		{
			;
		}
		GPIO_PORTA_DIR_R |=0x04; // PA2 as output 
		GPIO_PORTA_DIR_R &= ~(0x08); // PA3 as input
		
		GPIO_PORTA_DEN_R |=0x0C; // PA2 and PA3 Enable Digital Mode 
		GPIO_PORTA_AMSEL_R &= ~(0x08); // Diable the analog Mode PA0 to PA4
		GPIO_PORTA_AFSEL_R &=(~0x08); // Disable alternate funcs PA0 to PA4
		// PA3 Pull-down  
		// makes value to 1 -- ON state for the Postive switche Attached to kernel masters board
		GPIO_PORTA_PDR_R &=~(0x08); // making the switch to off state
		
		/*GPIO_PORTA_LOCK_R |=0x4C4F434B; // unlock  PF0 PIN from GPIOLOCK
		GPIO_PORTA_CR_R |=0x01;*/
}

void delay(void)
{
		int i=320000;
		while(i>=0)
			i--;
}

int main(void)
{
		PORT_PA_INIT(); // Green LED On
		
	  while(1)
		{
			GPIO_PORTA_DATA_R &= ~(0x04); // Switch off all LEDs
			if(!(GPIO_PORTA_DATA_R&0x08)==0) // SW 
				{
					GPIO_PORTA_DATA_R |= 0x04; // Red LED
				delay();
					GPIO_PORTA_DATA_R &= ~(0x04); // Red LED
				
					// GPIO_PORTA_DATA_R ^= GPIO_PORTA_DATA_R;
			}
			
			
		}
		/*
				  while(1)
		{
			GPIO_PORTA_DATA_R &=~(0x04); // Switch off all LEDs
			
			while(!(GPIO_PORTA_DATA_R&0x08)==0) // SW 
			{
				delay();
					GPIO_PORTA_DATA_R &= ~(0x04); // Red LED
				delay();
					GPIO_PORTA_DATA_R |=(0x04);
			}*/
}
