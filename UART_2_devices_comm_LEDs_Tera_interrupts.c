#include "km_tm4c123gh6pm.h"
#include "pll.h"
#include "kmlib1.h"


// Clock intialization
void RCC_INIT()
{
		// PORT F
		SYSCTL_RCGCGPIO_R |= 0x20; // enabling clock pulses to RCGCGPIO
		
		while(!(SYSCTL_PRGPIO_R & 0x20)) //Checking if the register is all set to take in the clock pulses
		{
			;
		}
		
		// PORT C pins for UART communication
		SYSCTL_RCGCGPIO_R |= 0x04;
		
		while((SYSCTL_PRGPIO_R & 0x04)!=0x04)
		{
			;
		}
		
		// UART
		SYSCTL_RCGCUART_R |= (0x02); // To give UART pin-1 clock pulse
		while((SYSCTL_PRUART_R & 0x02)!=0x02)
		{
			;
		}
}


void SYSTICK_TIMER_INIT()
{
	 // Enable Sys Clock
	 NVIC_ST_CTRL_R |= 0x04;
	
	 NVIC_ST_CTRL_R &= ~(0x01);
	 NVIC_ST_CURRENT_R = 123;
	 NVIC_ST_CTRL_R |=0x01; 
}

void PORT_PF_INIT(void)
{
		// Remove the lock of PF0
		GPIO_PORTF_LOCK_R |=0x4C4F434B; // unlock  PF0 PIN from GPIOLOCK
		GPIO_PORTF_CR_R |=0x01;	
		
		GPIO_PORTF_DEN_R |=0x17; // PF0,PF1,PF2 and PF4 Digital Mode
		GPIO_PORTF_DIR_R |=0x06; // PF1 PF2 to set o/p DIR
		GPIO_PORTF_DIR_R &= ~(0x11); // PF0 and PF4 to set i/p DIR

		GPIO_PORTF_AMSEL_R &= ~(0x17); // Diable the analog Mode PF0,PF1,PF2 and PF4
		GPIO_PORTF_AFSEL_R &= ~(0x17); // Disable alternate funcs PF0,PF1,PF2 and PF4
		
		// PF0 and PF4 enable Pull-up -- 
		// makes value to 1 -- OFF state for the negative switches like these
		GPIO_PORTF_PUR_R |=0x11; 
}

void GPIO_PORTF_INTERRUPTS_ENABLE()
{
		NVIC_EN0_R |= 0x40000000; //to enable the interrupt for port-F
		
	  GPIO_PORTF_IM_R &= ~(0x11); // mask the interrupt
	
		GPIO_PORTF_IS_R &= ~(0x1F); // Edge sensitive
		GPIO_PORTF_IBE_R &= ~(0x1F); // clear the control, we don't need interrupt for both rising and falling
	  GPIO_PORTF_IEV_R &= ~(0x1F); // falling edge
		
   	GPIO_PORTF_IM_R |= (0x11); // UnMask the interrupt.. send the interrupt to the interrupt controller
}

/**
UART function calls
*/
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

void UART_INTERRUPTS_ENABLE()
{
		// Enable UART in NVIC_EN_0
	
		NVIC_EN0_R |= 0x40; //to enable the interrupt for UART
		
		
		UART1_IM_R &= ~(0x10); // clear the interrupts
		
		UART1_ICR_R |= (0x10);  // clear the value of 4th bit in ICR
	
		UART1_IM_R |= 0x10; // Enable interrupt for 4th Bit		
}

/**
		Interrupt Handlers
**/
// Interrupt handler for falling edge
void GPIOF_Handler()
{
		// GPIO_PORTF_DATA_R ^= ~(0x01); // Switch off all LEDs
		// KM_delay_ms(100);
		// GPIO_PORTF_DATA_R ^= ~(0x01); // Switch off all LEDs
	
	// 2nd Switch - PF0
	if((GPIO_PORTF_RIS_R&0x01==0x01))
	{
		if((UART1_FR_R&(0x20))==0x20) // transmitter buffer/FIFO is full
		{
			return;
		}
		UART1_DR_R = 'R';
	}
	else //PF4
	{
		if((UART1_FR_R&(0x20))==0x20) // transmitter buffer/FIFO is full
		{
			return;
		}
		UART1_DR_R = 'B';
	}
	GPIO_PORTF_ICR_R |= (0x11);
}

void UART1_Handler()
{
		char y;
		
			y=UART1_DR_R;
				if(y=='R')
				{
						GPIO_PORTF_DATA_R &= ~(0x06);
						GPIO_PORTF_DATA_R |= 0x02;
						KM_delay_ms(100);
						GPIO_PORTF_DATA_R &= ~(0x02);
				}
				if(y=='B')
				{
						GPIO_PORTF_DATA_R &= ~(0x06);
						GPIO_PORTF_DATA_R |= 0x04;
						KM_delay_ms(100);
						GPIO_PORTF_DATA_R &= ~(0x04);
				
				}
}


void 	EnableInterrupts(void); // to enable interrupts as by defaults interrupts are off
void WaitForInterrupt(void); // to save power

int main(void)
{
	
		RCC_INIT();
	
		// Initialize PLL for the 
		KM_PLL_Init();
		SYSTICK_TIMER_INIT();
		
	  PORTC_init(); // Enable GPIO pins and assign AFSEL + DEN + GPIOPCTL
	
		PORT_PF_INIT();
		CONFIGURE_UART();
	
		GPIO_PORTF_INTERRUPTS_ENABLE();
		UART_INTERRUPTS_ENABLE();
	
		EnableInterrupts();
		
	  while(1)
		{
				WaitForInterrupt(); // just to keep the code running
		}
		return 0;
}