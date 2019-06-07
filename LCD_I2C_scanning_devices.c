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

/******************* LCD functions start **********/
void enable()
{
	 GPIO_PORTA_DATA_R |= 0x40;
	 KM_delay_us(10000);
	 GPIO_PORTA_DATA_R &= ~(0x40);
}

void higher_nibble(char c)
{
		GPIO_PORTD_DATA_R = c>>4;
	enable();
}

void lower_nibble(char c)
{
		GPIO_PORTD_DATA_R = c;
	enable();
}

void write_data(char c)
{
	// set RS=1
	// command writing to data-pins
	
	GPIO_PORTA_DATA_R |= (0x80);
	
	higher_nibble(c);
	lower_nibble(c);
}


void write_cmd(char c)
{
	// set RS=0
	// command writing to data-pins
	
	GPIO_PORTA_DATA_R &= ~(0x80);
	
	higher_nibble(c);
	lower_nibble(c);

}

void GPIO_PORTD_PORTA_ENABLE()
{
	   SYSCTL_RCGCGPIO_R |= (0x01); // A and D ports 
		 //while((SYSCTL_PRGPIO_R&0x09)!=0x09)
		 while(!(SYSCTL_PRGPIO_R & 0x01)) // checking Peripheral Ready Register PRGPIO
		 {
				;
		 }
		 SYSCTL_RCGCGPIO_R |= 0x08; // A and D ports 
		 //while((SYSCTL_PRGPIO_R&0x09)!=0x09)
		 while(!(SYSCTL_PRGPIO_R&0x08)) // checking Peripheral Ready Register PRGPIO
		 {
				;
		 }
		 GPIO_PORTA_DEN_R |=0xE0; // PA5-7 to enable digital mode
		 GPIO_PORTA_DIR_R |=0xE0; // PA5,PA6,PA7 as output
		 GPIO_PORTA_AMSEL_R &= ~(0xE0); // Diable the analog Mode PA5 and PF7
		 GPIO_PORTA_AFSEL_R &=~(0xE0); // Disable alternate funcs PF5 and PF7
		 
     GPIO_PORTD_DEN_R |=0x0F; // PD0,PD1,PD2,PD3 to enable digital mode
		 GPIO_PORTD_DIR_R |=0x0F; // PD0,PD1,PD2,PD3 as output
		 GPIO_PORTD_AMSEL_R &= ~(0x0F); // Diable the analog Mode PD0,PD1,PD2,PD3
		 GPIO_PORTD_AFSEL_R &= ~(0x0F); // Disable alternate funcs PD0,PD1,PD2,PD3
}

void initialize_LCD_for_4bit_input()
{
		// Set RW to 0
	  GPIO_PORTA_DATA_R &= ~(0xE0);

	/*
	  INITIALIZATION STEPS:(to convert 8-bit data pins to 4-bit data pins)
		1. delay(20ms)
		2. GPIO_PORT_D = 0x3; enable();
		3. delay(7ms)
		4. GPIO_PORT_D = 0x3; enable();
		5. delay(130us)
		6. GPIO_PORT_D = 0x3; enable();
		7. GPIO_PORT_D = 0x2; enable();
		8. GPIO_PORT_D = 0x1; enable();
*/
    KM_delay_us(20000); 
		write_cmd(0x30);
		enable();
	
		KM_delay_us(5000);
		write_cmd(0x30);
		enable();
	
		KM_delay_us(120);
		write_cmd(0x30);
		enable();
		
		write_cmd(0x20);
		enable();
		
		write_cmd(0x01);
		
		write_cmd(0x80);
}




int main()
{
		int result[128],i,j,temp;
		char data_from_slave;
		KM_PLL_Init();
	  SYSTICK_TIMER_INIT();
		RCC_INIT();
	
		GPIO_PORTD_PORTA_ENABLE();
	initialize_LCD_for_4bit_input();
	
		PORTB_init();
		
		for(i=0;i<128;i++)
		{
			Initialize_master(i);
			Initialize_slave(i);
			
		
			transmit_data_from_master('A');
			result[i] = check_data_status_in_master();	
			
			write_cmd(0x01); // clear screen
			write_cmd(0x80); // get the cursor to line 1 forcefully
			if(i<100)
			{
				j = (i/10)+48;
				write_data(j);
				j = (i%10)+48;
				write_data(j);
			}
			else // i>=100
			{
				j = (i/100)+48;
				write_data(j);
				
				j = (i%100);
				if(j<10)
				{	
					write_data('0');
					temp = j+48;
					write_data(temp);	
				}
				else if(j>=10)
				{
					temp = (j/10)+48;
					write_data(temp);
					temp = (j%10)+48;
					write_data(temp);
				}			
			}
			if(result[i]==-1)
			{
				  write_data(' ');
					write_data('-');
					write_data(' ');
					
					write_data('N');
					write_data('O');
					write_data('T');
					write_data(' ');
					write_data('F');
					write_data('O');
					write_data('U');
					write_data('N');
					write_data('D');
			}
			else
			{
					write_data(' ');
					write_data('-');
					write_data(' ');
					write_data('F');
					write_data('O');
					write_data('U');
					write_data('N');
					write_data('D');
			}
			KM_delay_ms(300);					KM_delay_ms(300);					KM_delay_ms(300);
	  }
		
		write_cmd(0xC0);
		write_data(' ');
		write_data(' ');
		write_data(' ');

		write_data('C');
		write_data('O');
		write_data('M');
		write_data('P');
		write_data('L');
		write_data('E');
		write_data('T');
		write_data('E');
		write_data('D');

		
		return 0;
}