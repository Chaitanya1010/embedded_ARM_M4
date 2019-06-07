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


int main()
{
		int temp_main=0, temp_fraction=0, humidity_temp=0, humidity_fraction=0,parity=0;
		KM_PLL_Init();
	  SYSTICK_TIMER_INIT();
		PORTE_init();

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
				
		}
	
		return 0;
}