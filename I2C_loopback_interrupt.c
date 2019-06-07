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

void Initialize_master()
{
		I2C0_MCR_R |= 0x00000031; // enabling 4th bit so that master function is enabled and the 0th bit for loopback
		I2C0_MTPR_R = 0x18; // the value is 23
		I2C0_MSA_R  = 0xE0; // assigning 1st bit for slave address
		I2C0_MSA_R  &= ~(0x01); // R/S byte to state that master is transmitting
}

void transmit_data_from_master(char x)
{
		I2C0_MDR_R = x; // writing data from master end
		I2C0_MCS_R = 0x07; // Assigning START STOP RUN BITS
}

void Initialize_slave_with_interrupts()
{
		I2C0_SOAR_R = 0x70; // slave address (slave address in master/2)
		I2C0_SCSR_R = 0x01; // to enable slave operation
			
		I2C0_SIMR_R = 0x01; // enable interrupts in Slave 
		NVIC_EN0_R |=0x0100; // enable the interrupt for I2C
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


void I2C0_Handler(void)
{
		if((I2C0_SCSR_R&0x01)== 0x01) // wait for RREQ bit to become 1
		{
					// now the RREQ bit is 1
			if((I2C0_SCSR_R&0x04)== 0x04) // wait for FBR bit to become 1
			{
				
				y = I2C0_SDR_R;
				I2C0_SICR_R |= 0x01; // clear the interrupt that has occured.
			}
		}
}

void EnableInterrupts(void);

void WaitForInterrupt(void);

int main()
{
		int result;
		char data_from_slave;
		KM_PLL_Init();
	  SYSTICK_TIMER_INIT();

		RCC_INIT();
	
		PORTB_init();
		
		Initialize_master();
		Initialize_slave_with_interrupts();
		
	
		transmit_data_from_master('A');
	  result = check_data_status_in_master();	
	  //data_from_slave = receive_data_in_slave();
	
		EnableInterrupts();
		while(1)
		{
				WaitForInterrupt();
		}
		
		return 0;
}

	