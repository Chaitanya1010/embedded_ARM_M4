#include "km_tm4c123gh6pm.h"
#include "kmlib1.h"
#include "pll.h"

int i;
char arr1[7]={0};


int SYSTICK_TIMER_INIT()
 {
	 NVIC_ST_CTRL_R |=0x4;//ENABLE SYSTEM CLOCK
	 NVIC_ST_CTRL_R &=~(0x01);///disable systicktimer 
	 NVIC_ST_CURRENT_R =3124;
  NVIC_ST_CTRL_R |=0x01;//ENABLE SYSTICK TIMER
	 return 0;
 }
 
void RCC_INIT(void)
{
	SYSCTL_RCGCGPIO_R |=0X02;   //ENABLING PORT B CLOCK
	while(!(	SYSCTL_PRGPIO_R & 0X02))
	{
		;
	}
	SYSCTL_RCGCI2C_R |=0X01;  ////ENABLING I2C CLOCK
	while(!(	SYSCTL_PRI2C_R & 0X01))
	{
		;
	}

		SYSCTL_RCGCGPIO_R |= (0x09);   //*(0x400FE608) is aphysical addr of RCGCGPIO,0x400FE000 AND 0X608 IS OFFSET ADDRESS
	 //TYPE CASTING IS USED
	 while(!SYSCTL_PRGPIO_R & 0x09)
	 {
		 ;
	 }
}

void I2C0_INIT()
{    
	 //ENABLE I2CSDA PIN
	 I2C0_MCR_R |=0X10; //ENABLE MASTER SLAVE AND I2C0 BIT AND LOOP BACK ALSO
	 I2C0_MTPR_R &=~(0X18); //CLEARING MTPR
	 I2C0_MTPR_R =24; //TPR=(50MHZ/2*(6+4)*100KBPS)-1 =24 (IN HEX=>18H)   //SETTING THE SPEED
}

void PORTB_INIT()
{
		GPIO_PORTB_AFSEL_R &= ~(0x0C); // Enable alternate funcs PB2 and PB3
		GPIO_PORTB_AFSEL_R |= (0x0C); // Enable alternate funcs PB2 and PB3
		
		//GPIOODR	
		GPIO_PORTB_ODR_R &= ~(0x08);
		GPIO_PORTB_ODR_R |= (0x08);
		GPIO_PORTB_DEN_R |= (0x0C); // Enable the Digital mode for PB2 and PB3
		
		GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)| 0x00003300;
	
		//GPIO_PORTB_DIR_R |= (0x08);
		GPIO_PORTB_AMSEL_R &= ~(0x0C); // Diable the analog Mode PB2 and PB3
}

void master_transmit()
{
		char arr[7]={0,8,2,9,37,05,24};
		
		// ENABLE START RUN  BIT
		I2C0_MCS_R =0X03; 
		
		for(i=0;i<7;i++)
		{
			//CHECKING BUSY BIT      
			while((I2C0_MCS_R&0x01)== 0x01)
			{
					;
			};
			// Saving slave address in MSA
			I2C0_MSA_R = (0x68<<1)&0xFE;
			// Clearing MSA first bit, stating that master is transmitting
			I2C0_MSA_R &= ~0x01;             
			// Adding data in MDR in loop
			I2C0_MDR_R = i&0xFF;         
			// Assigning START and RUN bits in MCS
			I2C0_MCS_R = 0x03;
			
			// Wait till I2C is busy
			while((I2C0_MCS_R&0x01)== 0x01)
			{
					;
			};
			// Adding data to MDR 
			I2C0_MDR_R = arr[i]&0xFF;
			// Setting STOP and RUN in MCS
			I2C0_MCS_R = 0x05;
			
			// Waiting for BUSY Bit to clear
			while((I2C0_MCS_R&0x01)==0x01)
			{
				 ;
			};

//			// Checking error bit
//			if((I2C0_MCS_R&0x02)==0x02)
//			{
//					return -1
//			}	
		}
}

void master_receive()
{
		char data;

		for(i=0;i<7;i++)
		{
			// REPEATED START
			// Use the prev transmission configuration here
			
			// Waiting for BUSY Bit to clear
			while((I2C0_MCS_R&0x01)==0x01)
			{
				 ;
			};
			// Assign Slave address in MSA
			I2C0_MSA_R = (0x68<<1)&0xFE;  
			// Clear the 0th bit in MSA stating that Master is transmitting 
			I2C0_MSA_R &= ~0x01;             
			// Assign data in MDR
			I2C0_MDR_R = (i)&0xFF;
			// Assign Start, Stop and Run bits in MCS
			I2C0_MCS_R = 0x03;
			// Waiting for BUSY Bit to clear
			while((I2C0_MCS_R&0x01)==0x01)
			{
				 ;
			};  
			
			// Start the new configuration from here
			
			// Waiting for BUSY Bit to clear
			while((I2C0_MCS_R&0x01)==0x01)
			{
				 ;
			};
			// Assign Slave address in MSA
			I2C0_MSA_R = (0x68<<1)&0xFE;    
			// Setting the 0th bit in MSA to 1 stating that Master is receiving 
			I2C0_MSA_R |= 0x01;         
			// Set Start Stop and Run bits
			I2C0_MCS_R = 0x07;	
			 
			// Waiting for BUSY Bit to clear
			while((I2C0_MCS_R&0x01)==0x01)
			{
				 ;
			};
			// get the data from MDR
			arr1[i]=(I2C0_MDR_R&0xFF);
			arr1[i]=(arr1[i]-6*(arr1[i]>>4));
		}
}

int main()
{

		int ret,i;
		KM_PLL_Init();
		SYSTICK_TIMER_INIT();
		RCC_INIT();
		PORTB_INIT();
	
		I2C0_INIT();

		master_transmit();
	
		while(1)
		{
				master_receive();
				KM_delay_ms(200);
				KM_delay_ms(200);
				KM_delay_ms(200);
				KM_delay_ms(200);
				KM_delay_ms(200);
		}
}