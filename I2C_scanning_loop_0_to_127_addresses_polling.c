#include "km_tm4c123gh6pm.h"
#include "pll.h"
#include "kmlib1.h"

char y='O';

void SYSTICK_TIMER_INIT()
{
	// Enable Sys Clock
	 NVIC_ST_CTRL_R &= ~(0x01);
	 NVIC_ST_CTRL_R |= 0x04;
	 NVIC_ST_CURRENT_R = 123;
	 NVIC_ST_CTRL_R |=0x01; 
}

void PORTB_init()
{
	// UnLock B2 and B3
	// and to commit the change using CR
		GPIO_PORTB_LOCK_R |= 0x4C4F434B;
		GPIO_PORTB_CR_R |= 0x0C;

		GPIO_PORTB_DEN_R |= (0x0C); // Enable the Digital mode for PB2 and PB3
	//	GPIO_PORTB_DIR_R |= (0x08);
		GPIO_PORTB_AMSEL_R &= ~(0x0C); // Diable the analog Mode PB2 and PB3
		GPIO_PORTB_AFSEL_R &= ~(0x0C); // Enable alternate funcs PB2 and PB3
		GPIO_PORTB_AFSEL_R |= (0x0C); // Enable alternate funcs PB2 and PB3
		//GPIOODR	
		GPIO_PORTB_ODR_R &= ~(0x08);
		GPIO_PORTB_ODR_R |= (0x08);
		GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xffff00ff)| 0x00003300;
		
}
	


void RCC_INIT()
{
		SYSCTL_RCGCI2C_R |= (0x01); // To give I2C module 0 clock pulse
		while((SYSCTL_PRI2C_R & 0x01)!=0x01)
		{
			;
		}
		
		// PORT B register clock pulse communication
		SYSCTL_RCGCGPIO_R |= 0x02;
		
		while((SYSCTL_PRGPIO_R & 0x02)!=0x02)
		{
			;
		}	
}

void Initialize_master(int i)
{
		I2C0_MCR_R |= 0x00000010; // enabling 4th bit so that master function is enabled
		I2C0_MTPR_R = 0x18; // the value is 23
		I2C0_MSA_R  = i<<1; // assigning 1st bit for slave address
		I2C0_MSA_R  &= ~(0x01); // R/S byte to state that master is transmitting
}

void transmit_data_from_master(char x)
{
		I2C0_MDR_R = x; // writing data from master end
		I2C0_MCS_R = 0x07; // Assigning START STOP RUN BITS
}

void Initialize_slave(int i)
{
		I2C0_SOAR_R = i; // slave address (slave address in master/2)
		I2C0_SCSR_R = 0x01; // to enable slave operation
}

int check_data_status_in_master()
{
		while((I2C0_MCS_R&0x01)==0x01) // wait since the IC2 is busy
		{
				;
		}
		
		if((I2C0_MCS_R&0x02)==0x02) // wait and check the error bit
		{
				// the error exists
				return -1;
		}
		return 0;
}


int main()
{
		int result[128],i;
		char data_from_slave;
		KM_PLL_Init();
	  SYSTICK_TIMER_INIT();

		RCC_INIT();
	
		PORTB_init();
		
		for(i=0;i<128;i++)
		{
			Initialize_master(i);
			Initialize_slave(i);
			
		
			transmit_data_from_master('A');
			result[i] = check_data_status_in_master();	
	  }
		
		
		return 0;
}

	