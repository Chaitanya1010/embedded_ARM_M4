#include "km_tm4c123gh6pm.h"
#include "pll.h"
#include "kmlib1.h"


void SYSTICK_TIMER_INIT()
{
	// Enable Sys Clock
	 NVIC_ST_CTRL_R &= ~(0x01);
	 NVIC_ST_CTRL_R |= 0x04;
	 NVIC_ST_CURRENT_R = 123;
	 NVIC_ST_CTRL_R |=0x01; 
}

void PORTE_init()
{
		GPIO_PORTE_DEN_R |=0x02; // PE1 Digital Mode

		GPIO_PORTE_AMSEL_R &= ~(0x02); // Diable the analog Mode PE1
		GPIO_PORTE_AFSEL_R &= ~(0x02); // Disable alternate funcs PE1
}

void make_request_to_dht11()
{
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
		int i,var=0,arr[45];
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
						
					/*	if(i==0)
						{
								var = 0x01; 
						}
						else
						{
								var = var << 1;
								var |= 0x01;
						}*/
					var = (var<<1)|1;
					arr[i]=1;
						while((GPIO_PORTE_DATA_R&0x02))
						{
								;// the input is still high
						}
				}
				else
				{
					/*	if(i==0)
						{
								var = 0x00; 
						}
						else
						{
								var = var << 1;
						}*/
					arr[i]=0;
					var = (var<<1)|0;
				}			
		}
		return var;

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
		
		// PORT E pins for UART communication
		SYSCTL_RCGCGPIO_R |= 0x10;
		
		while(!(SYSCTL_PRGPIO_R & 0x10))
		{
			;
		}
		
		
}


void PORTC_init()
{
		GPIO_PORTC_DEN_R |= 0x30; // Enable Digital Mode for C4-C5
		
		GPIO_PORTC_AMSEL_R &= ~(0x30); // Disable Analog Mode for C4-C5
		GPIO_PORTC_AFSEL_R |= 0x30; // Enable Alternate Mode for C4-C5
		
		GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xff00ffff)| 0x00220000; //32 bits reg PCTL is configured this way with 32 bits (8bytes)hence 8 blocks
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

void SEND_DATA_UART(char x)
{
		if((UART1_FR_R&(0x20))==0x20) // transmitter buffer/FIFO is full
		{
			return;
		}
		UART1_DR_R = x;
	}

int main()
{
	char i;
		int temp_main=0, temp_fraction=0, humidity_temp=0, humidity_fraction=0,parity=0;
		KM_PLL_Init();
	  SYSTICK_TIMER_INIT();
	
		RCC_INIT();
	
		PORTE_init();
		PORTC_init();
	
		CONFIGURE_UART();

		KM_delay_ms(2000); // wait for 2 Second before we start
		
		make_request_to_dht11();
		
		get_response_from_dht11();
		
		humidity_temp = get_data_from_dht11();
		humidity_fraction = get_data_from_dht11();
		temp_main = get_data_from_dht11();
		temp_fraction = get_data_from_dht11();
		parity = get_data_from_dht11();
	
		if(parity==temp_main+temp_fraction+humidity_temp+humidity_fraction)
		{
				// UART_SEND_DATA function
			SEND_DATA_UART('T');KM_delay_ms(5);
			SEND_DATA_UART('E');KM_delay_ms(5);
			SEND_DATA_UART('M');KM_delay_ms(5);
			SEND_DATA_UART('P');KM_delay_ms(5);
			SEND_DATA_UART(':');KM_delay_ms(5);
			i = (temp_main/10)+48;
			SEND_DATA_UART(i);KM_delay_ms(5);
			i = (temp_main%10)+48;
			SEND_DATA_UART(i);KM_delay_ms(5);
			SEND_DATA_UART('.');KM_delay_ms(5);
			i = (temp_fraction/10)+48;
			SEND_DATA_UART(i);KM_delay_ms(5);
			SEND_DATA_UART(i);KM_delay_ms(5);
			SEND_DATA_UART('\n');
			SEND_DATA_UART('\b');KM_delay_ms(5);
			SEND_DATA_UART('\b');KM_delay_ms(5);
			SEND_DATA_UART('\b');KM_delay_ms(5);
			SEND_DATA_UART('\b');KM_delay_ms(5);
			SEND_DATA_UART('\b');KM_delay_ms(5);
			SEND_DATA_UART('\b');KM_delay_ms(5);
			SEND_DATA_UART('\b');KM_delay_ms(5);
			SEND_DATA_UART('\b');KM_delay_ms(5);
			SEND_DATA_UART('\b');KM_delay_ms(5);
			SEND_DATA_UART('\b');KM_delay_ms(5);
			KM_delay_ms(5);
			SEND_DATA_UART('H');KM_delay_ms(5);
			SEND_DATA_UART('U');KM_delay_ms(5);
			SEND_DATA_UART('M');KM_delay_ms(5);
			SEND_DATA_UART('I');KM_delay_ms(5);
			SEND_DATA_UART('D');KM_delay_ms(5);
			SEND_DATA_UART(':');KM_delay_ms(5);
			i = (humidity_temp/10)+48;
			SEND_DATA_UART(i);KM_delay_ms(5);
			i = (humidity_temp%10)+48;
			SEND_DATA_UART(i);KM_delay_ms(5);
			SEND_DATA_UART('.');KM_delay_ms(5);
			i = (humidity_fraction/10)+48;
			SEND_DATA_UART(i);KM_delay_ms(5);
				
		}
	
		return 0;
}