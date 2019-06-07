#include "km_tm4c123gh6pm.h"
#include "pll.h"
#include "kmlib1.h"


// to set pulse high and low with 10ms delay
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

void write_cmd(char c)
{
	// set RS=0
	// command writing to data-pins
	
	GPIO_PORTA_DATA_R &= ~(0x80);
	
	higher_nibble(c);
	lower_nibble(c);

}

void write_data(char c)
{
	// set RS=1
	// command writing to data-pins
	
	GPIO_PORTA_DATA_R |= (0x80);
	
	higher_nibble(c);
	lower_nibble(c);
}

void SYSTICK_TIMER_INIT()
{
	// Enable Sys Clock
	NVIC_ST_CTRL_R |= 0x04;
	 NVIC_ST_CTRL_R &= ~(0x01);
	 
	 NVIC_ST_CURRENT_R = 123;
	 NVIC_ST_CTRL_R |=0x01; 
}

void PORTE_init()
{
		SYSCTL_RCGCGPIO_R |= 0x10;
		
		while(!(SYSCTL_PRGPIO_R & 0x10))
		{
			;
		}
		GPIO_PORTE_DEN_R |=0x02; // PE1 Digital Mode

		GPIO_PORTE_AMSEL_R &= ~(0x02); // Diable the analog Mode PE1
		GPIO_PORTE_AFSEL_R &= ~(0x02); // Disable alternate funcs PE1
}

void make_request_to_dht11()
{
	  KM_delay_ms(2000);
	  GPIO_PORTE_DIR_R |=0x02; // PE1 set o/p DIR
	
		GPIO_PORTE_DATA_R |= 0x02; // set output as 1
		GPIO_PORTE_DATA_R = 0x00;	// change output to 0
		KM_delay_ms(20); // delay for 18-20ms
		GPIO_PORTE_DATA_R |= 0x02;	// change output to 1
	
		GPIO_PORTE_DIR_R &=~(0x02); // PE1 set Direction to Input
}		

void get_response_from_dht11()
{
		while((GPIO_PORTE_DATA_R&0x02))
		{
			; //input is still high
		}
		while((GPIO_PORTE_DATA_R&0x02)==0)
		{
			; // input is still low
		}
		while((GPIO_PORTE_DATA_R&0x02))
		{
			; //input is still high
		}
}

int get_data_from_dht11()
{
		int i,var=0;
		// wait till the input is low 
		for(i=0;i<8;i++)
		{
				while(!(GPIO_PORTE_DATA_R&0x02))  // 50us
				{
					; // input is still low
				}
				
				// wait for 30 us
				KM_delay_us(30);

				//check the value in PE1
				
				if((GPIO_PORTE_DATA_R&0x02))
				{
					var = (var<<1)|1;
						while((GPIO_PORTE_DATA_R&0x02))
						{
								;// the input is still high
						}
				}
				else
				{
					var = (var<<1)|0;
				}			
		}
		return var;

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
		char i;
		int temp_main=0, temp_fraction=0, humidity_temp=0, humidity_fraction=0,parity=0;
		GPIO_PORTD_PORTA_ENABLE();
		KM_PLL_Init();
	  SYSTICK_TIMER_INIT();
		PORTE_init();
		initialize_LCD_for_4bit_input();	
		KM_delay_ms(2000); // wait for 2 Second before we start
		
		while(1)
		{			
			KM_delay_ms(300);
			KM_delay_ms(300);
			KM_delay_ms(300);
			KM_delay_ms(300);
			KM_delay_ms(300);
			KM_delay_ms(300);
			KM_delay_ms(300);
			KM_delay_ms(300);
			KM_delay_ms(300);
			KM_delay_ms(300);
			KM_delay_ms(300);
			KM_delay_ms(300);
			
			make_request_to_dht11();
			
			get_response_from_dht11();
			
			humidity_temp = get_data_from_dht11();
			humidity_fraction = get_data_from_dht11();
			temp_main = get_data_from_dht11();
			temp_fraction = get_data_from_dht11();
			parity = get_data_from_dht11();
			
			write_cmd(0x01);
			write_cmd(0x80);
		
			if(parity==temp_main+temp_fraction+humidity_temp+humidity_fraction)
			{
					write_data('T');
					write_data('E');
					write_data('M');
					write_data('P');
					write_data(':');
					i = (temp_main/10)+48;
					write_data(i);
					i = (temp_main%10)+48;
					write_data(i);
					write_data('.');
					i = (temp_fraction/10)+48;
					write_data(i);
					write_data(i);
					write_cmd(0xC0);
					write_data('H');
					write_data('U');
					write_data('M');
					write_data('I');
					write_data('D');
					write_data(':');
					i = (humidity_temp/10)+48;
					write_data(i);
					i = (humidity_temp%10)+48;
					write_data(i);
					write_data('.');
					i = (humidity_fraction/10)+48;
					write_data(i);
				}
		}
		return 0;
}