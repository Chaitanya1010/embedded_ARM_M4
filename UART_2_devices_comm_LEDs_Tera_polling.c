#include "km_tm4c123gh6pm.h"
#include "pll.h"
#include "kmlib1.h"


void SYSTICK_TIMER_INIT()
{
	 // Enable Sys Clock
	 NVIC_ST_CTRL_R |= 0x04;
	
	 NVIC_ST_CTRL_R &= ~(0x01);
	 NVIC_ST_CURRENT_R = 123;
	 NVIC_ST_CTRL_R |=0x01; 
}

void PORTC_init()
{
		GPIO_PORTC_DEN_R |= 0x30; // Enable Digital Mode for C4-C5
		
		GPIO_PORTC_AMSEL_R &= ~(0x30); // Disable Analog Mode for C4-C5
		GPIO_PORTC_AFSEL_R |= 0x30; // Enable Alternate Mode for C4-C5
		
		GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xff00ffff)| 0x00220000;
}

void CONFIGURE_UART()
{
		// Disable UARTEN to disable all UART functions
		UART1_CTL_R &= ~(0x01); // Since PC4 and 5 are connected to UART 1
	
		// float BRD = (50*1000000)/(16*9600);
		UART1_IBRD_R = 325; // Storing Integer value in IBRD
		UART1_FBRD_R = 33; // Storing Integer value in IBRD
	
		UART1_LCRH_R |= 0x60; // assinging the word-length values to LCRH
			
		// Enable software loopback
		// UART1_CTL_R |= 0x80;
	
		UART1_CTL_R |= (0x01);
}

void PORT_PF_INIT(void)
{
		GPIO_PORTF_DEN_R |=0x17; // PF0,PF1,PF2 and PF4 Digital Mode
		GPIO_PORTF_DIR_R |=0x06; // PF1 PF2 to set o/p DIR
		GPIO_PORTF_DIR_R &= ~(0x11); // PF0 and PF4 to set i/p DIR

		GPIO_PORTF_AMSEL_R &= ~(0x17); // Diable the analog Mode PF0,PF1,PF2 and PF4
		GPIO_PORTF_AFSEL_R &= ~(0x17); // Disable alternate funcs PF0,PF1,PF2 and PF4
		
		// PF0 and PF4 enable Pull-up -- 
		// makes value to 1 -- OFF state for the negative switches like these
		GPIO_PORTF_PUR_R |=0x11; 
		// GPIO_PORTF_DATA_R |=0x08; // Sets Green color
		
		GPIO_PORTF_LOCK_R |=0x4C4F434B; // unlock  PF0 PIN from GPIOLOCK
		GPIO_PORTF_CR_R |=0x01;		
}

void SEND_DATA_UART(char x)
{
		if((UART1_FR_R&(0x20))==0x20) // transmitter buffer/FIFO is full
		{
			return;
		}
		UART1_DR_R = x;
}

char RECEIVE_DATA_UART()
{
		char y;
	  // The receiver FIFO/Buffer is empty
		if((UART1_FR_R&(0x10))!=0x10)  // async way
		{
				y=UART1_DR_R;
				return y;
		}
		return 'O';
}

void RCC_INIT()
{
		SYSCTL_RCGCUART_R |= (0x02); // To give UART pin-1 clock pulse
		while((SYSCTL_PRUART_R & 0x02)!=0x02)
		{
			;
		}
		
		// PORT C pins for UART communication
		SYSCTL_RCGCGPIO_R |= 0x04;
		
		while((SYSCTL_PRGPIO_R & 0x04)!=0x04)
		{
			;
		}
		
		
		// Port F initialization
		SYSCTL_RCGCGPIO_R |= 0x20;
		
		while((SYSCTL_PRGPIO_R & 0x20)!=0x20)
		{
			;
		}
		
}

int main()
{
		int switch1_enable=0,switch2_enable=0;
		char output;
		
		RCC_INIT();
	
		// INITIALIZATION
		KM_PLL_Init();
	  SYSTICK_TIMER_INIT();

		PORTC_init(); // Enable GPIO pins and assign AFSEL + DEN + GPIOPCTL
		PORT_PF_INIT();
	
		// CONFIGURATION
		CONFIGURE_UART();

		//KM_delay_ms(2000); // wait for 2 Second before we start
		
		while(1)
		{
			
			// listening to the input of UART
			output = RECEIVE_DATA_UART();
			
			if(output == 'R')
			{
								GPIO_PORTF_DATA_R &= ~(0x06);
					GPIO_PORTF_DATA_R |= 0x06;
							KM_delay_ms(100);
				KM_delay_ms(300);
				KM_delay_ms(300);
				
			}
			else if(output == 'B')
			{
								GPIO_PORTF_DATA_R &= ~(0x06);
					GPIO_PORTF_DATA_R |= 0x02;
							KM_delay_ms(100);
				KM_delay_ms(300);
				KM_delay_ms(300);
			}
			else if(output == 'O')
			{
					KM_delay_ms(300);
			}
			else
			{
				GPIO_PORTF_DATA_R &= ~(0x06);
				GPIO_PORTF_DATA_R |= 0x04;
							KM_delay_ms(300);
				KM_delay_ms(300);
				KM_delay_ms(300);
			}
			
			
			// switch pressing logic
			if((GPIO_PORTF_DATA_R&0x10)==0x10) // SW1 ON
			{
					//Call UART write method
					SEND_DATA_UART('R');
			}
			
			
			
			if((GPIO_PORTF_DATA_R&0x01)==0x01) // SW2 ON
			{
					
					//Call UART write method
					SEND_DATA_UART('B');
			}
		}
		return 0;
}
