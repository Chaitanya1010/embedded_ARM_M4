#include "km_tm4c123gh6pm.h"
#include "pll.h"
#include "kmlib1.h"
int i,a,b,c,d,e,x,s,f,g,A,B,C,D,X,Y,U,V,W,E,F,Z,P,Q,R,S,G,H;
int Hum_Hi = 0, Hum_Lo = 0, Temp_Hi = 0, Temp_Lo = 0, Checksum = 0,count=0;
 char arr1[7]={0};
 char temp2;
 char data[5];
 char arr2[8];
void LCD_Write_Data(char);
void system_clock(void)
{
	SYSCTL_RCGCGPIO_R |=0X02;                //ENABLING PORT B CLOCK
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
int PORTD_Init(void)
{ int j;
	//it untill gpio portf pripheral is ready
  
	
	  GPIO_PORTD_LOCK_R |=0X4C4F434B;
	 GPIO_PORTD_CR_R |=0X80;
	  GPIO_PORTD_PCTL_R =(GPIO_PORTD_PCTL_R & 0X00FFFFFF)|(0X11000000);//SELECT UART PORT AND MUX POSITION
    GPIO_PORTD_DEN_R |=(0XC0); //ENABLE PC4 PC5 DIGITAL PINS 
	 GPIO_PORTD_AFSEL_R |=(0XC0);// ENABLE 
	  GPIO_PORTD_AMSEL_R &=~(0XC0); //DISABLE ANALOG MODE
	 return 0;
 }
void PORTE_Init(void)
{
	SYSCTL_RCGCGPIO_R |= (0x10); 
	while( !(SYSCTL_PRGPIO_R & 0x10));
	//GPIO_PORTE_DIR_R |= 0x2;           //DANGER____________
	
	GPIO_PORTE_AFSEL_R &= ~(0x2);  //Disable alternate functionality
	GPIO_PORTE_DEN_R |=(0x2);      //Digital Enable
	GPIO_PORTE_AMSEL_R &= ~(0x2);  //Analog Mode disable
}
void PORTB_Init(void)
{       
	//////PORT B INITIALIZATION
	// GPIO_PORTB_DIR_R |=(0x0C);
	GPIO_PORTB_AFSEL_R |=(0x0C);
	 GPIO_PORTB_ODR_R |=(0x08);     
	
	 GPIO_PORTB_DEN_R |=(0x0C);
	 GPIO_PORTB_PCTL_R =(GPIO_PORTB_PCTL_R & 0XFFFF00FF)|(0X00003300);
   
	 GPIO_PORTB_AMSEL_R &=~(0x0C);
}
 int SYSCTL_Init_lcd(void)
 {
	 //wait untill gpio portf pripheral is ready
  GPIO_PORTD_DIR_R |=(0x0F); // SET THE PA5 TO PA7 AS OUTPUT OPERATION
	GPIO_PORTA_DIR_R |=(0xE0); // SET THE PD0 TO PD3 AS OUTPUT OPERATION
  GPIO_PORTD_AFSEL_R &=~(0x0F); //CLEAR PA5 TO PA7 DUE TO USING GPIO
	GPIO_PORTA_AFSEL_R &=~(0xE0); //CLEAR PD0 TO PD3 DUE TO USING GPIO
	GPIO_PORTD_DEN_R |=(0x0F); //ENABLING ALL PINS  
	GPIO_PORTA_DEN_R |=(0xE0); //ENABLING ALL PINS 
	GPIO_PORTD_AMSEL_R &=~(0x0F); //CLEAR PA5 TO PA7 DUE TO USING DIGITAL MODE
	GPIO_PORTA_AMSEL_R &=~(0xE0); //CLEAR PA5 TO PA7 DUE TO USING DIGITAL MODE

	 return 0;
}
void I2C_Init()
{    
	////ENABLE I2CSDA PIN
	
	 I2C0_MCR_R |=0X10;        //ENABLE MASTER SLAVE AND I2C0 BIT AND LOOP BACK ALSO
	 I2C0_MTPR_R &=~(0X18);           ////CLEARING MTPR
	 I2C0_MTPR_R =24;////TPR=(50MHZ/2*(6+4)*100KBPS)-1  =24 (IN HEX=18H)   //SETTING THE SPPED
//GIVING SLAVE ADDR IN MASTER SEVEN BIT
  

  
 }
int SysTickTimer_Init1(long int x)
 {
	 NVIC_ST_RELOAD_R=x;
	 while(!NVIC_ST_CTRL_R&0x10000)
	 {
		 ;
	 }
return 0;	 
 }
  int SysTickTimer_Init(long int x)
 {
	 NVIC_ST_CTRL_R |=0x4;//ENABLE SYSTEM CLOCK
	 NVIC_ST_CTRL_R &=~(0x01);///disable systicktimer 
	 NVIC_ST_CURRENT_R =3124;
	// NVIC_ST_RELOAD_R=5000000;
	 //CLEAR 
	 //100MSEC DELAY
  NVIC_ST_CTRL_R |=0x01;//ENABLE SYSTICK TIMER
	 return 0;
 }
 void delayMicroseconds(int delay)
{
	int count = delay*50;
	NVIC_ST_CURRENT_R=3214;
	NVIC_ST_RELOAD_R = count;
	while(!(NVIC_ST_CTRL_R&0x10000))
		{
			;
		}
	//NVIC_ST_CTRL_R&= ~(0x1);
}

void SW_delay(void)
{
	KM_delay_ms(200);
	KM_delay_ms(200);
	KM_delay_ms(200);
	KM_delay_ms(200);
	KM_delay_ms(200);
}

void DHT11_Init(void)
{
	// wait for getting DHT11 Ready

	SW_delay();                        // wait for dht11 to get ready from unstable state
}

void DHT11_Request(void)
{
	// Send Request signal from uP to DHT11
	GPIO_PORTE_DIR_R |= 0x2;           //OUTPUT DIRECTION PE1 initialization;
//	KM_delay_ms(20);
	//send LOW pulse for 18 ms for Request from master to slave
	GPIO_PORTE_DATA_R &= ~(0x2); 
	//delayMicroseconds(20000);          //wait for 18 ms
KM_delay_ms(20);
	GPIO_PORTE_DATA_R |=  (0x2);       //make pin High again
}

// return 1 - low ; return 0 - High 
int Check_Low()
{
	if (GPIO_PORTE_DATA_R & 0x02)
  return 0;
	else
	return 1;
}

int Check_High()
{
	if (GPIO_PORTE_DATA_R & 0x02)
  return 1;
	else
	return 0;
}

// return 0 - DEVICE FOUND; return 1 -DEVICE NOT FOUND
int DHT11_Response(void)
{
	volatile int x=0,count=0;
	GPIO_PORTE_DIR_R &= ~(0x2);           //INPUT DIRECTION PE1 initialization;
	while(Check_High())  // wait till the pin goes LOW
	{
   	
		 if(count>2000)
		 {
			 x=1;
			 return 1;
		 }
		 count++;
		
	}
	while(Check_Low());  // 80usec low pulse 
	while(Check_High()); // 80usec High pulse
	return x;
}

int DHT11_Rx_Data(void)
{
	int data = 0, bit = 0, i;
	for (i = 0; i < 8; i++)
	{
		//data = data << 1; 
	//	delayMicroseconds(40);
		 while((GPIO_PORTE_DATA_R & (0x02)) == 0)
		{;
		}		
	   KM_delay_us(30);		
		 if((GPIO_PORTE_DATA_R & (0x02)) == 2)
		 {
			 bit = 1;
			  data = (data << 1);
			 data |= bit;
			 while((GPIO_PORTE_DATA_R & (0x02)) == 2);
		 }
	   else
		 {
			 bit = 0;
			  data = (data << 1);
			 data |= bit;
		 }
}
	
	
return data;
}
 int DHT_RTC(void)
{
	a=arr1[0]%10;
  arr1[0]=arr1[0]/10;	
//	b=Temp_Hi%10;
	data[0]=a;
	data[1]=arr1[0];
	X=data[0]+48;
	Y=data[1]+48;
	
	b=arr1[1]%10;
  arr1[1]=arr1[1]/10;	
//	b=Temp_Hi%10;
	data[0]=b;
	data[1]=arr1[1];
	U=data[0]+48;
	V=data[1]+48;
	
	c=arr1[2]%10;
  arr1[2]=arr1[2]/10;	
//	b=Temp_Hi%10;
	data[0]=c;
	data[1]=arr1[2];
	W=data[0]+48;
	Z=data[1]+48;
	
	g=arr1[2]%10;
  arr1[3]=arr1[3]/10;	
//	b=Temp_Hi%10;
	data[0]=g;
	data[1]=arr1[3];
	E=data[0]+48;
	F=data[1]+48;
	
	d=arr1[4]%10;
  arr1[4]=arr1[4]/10;	
//	b=Temp_Hi%10;
	data[0]=d;
	data[1]=arr1[4];
	P=data[0]+48;
	Q=data[1]+48;
	
	e=arr1[5]%10;
  arr1[5]=arr1[5]/10;	
//	b=Temp_Hi%10;
	data[0]=e;
	data[1]=arr1[5];
	R=data[0]+48;
	S=data[1]+48;
	
	f=arr1[6]%10;
  arr1[6]=arr1[6]/10;	
//	b=Temp_Hi%10;
	data[0]=a;
	data[1]=arr1[6];
	G=data[0]+48;
	H=data[1]+48;
	return 0;
	 }
int temp_humd(void)
{
	x=Temp_Hi%10;
  Temp_Hi=Temp_Hi/10;	
//	b=Temp_Hi%10;
	arr2[0]=x;
	arr2[1]=Temp_Hi;
	A=arr2[0]+48;
	B=arr2[1]+48;
	
	
	s=Hum_Hi%10;
  Hum_Hi=Hum_Hi/10;	
	arr2[0]=s;
	arr2[1]=Hum_Hi;
	C=arr2[0]+48;
	D=arr2[1]+48;
	
	return 0;
 }	

void master_transmission()
{
		 I2C0_MSA_R  &=~(0x01);   
   I2C0_MSA_R  =(0x68)<<1; 	 //RTC ADDRESSS
	
	 I2C0_MDR_R  =0X00;//GIVING register address  VALUE OF TRANSMITT DATA TO THE DATA REG 
	
	 
}
void	LCD_Write_Data_Str(char *c)
				{
					int i=0;
				  for(i=0;c[i];i++)
            {	
               LCD_Write_Data(c[i]);
					}
						//UART1_Inchar();	
					   
														
				}
void MASTER_write()
{ //MASTER TRANSMISSION
	while((I2C0_MCS_R & 0x01)==1)  ///CHECKING BUSY BIT
	{
		;
	}
 if((I2C0_MCS_R & 0x02)==0)  ///CHECKING ERROR BIT
	{
		;
	}
}
void master_transmitt()
{
	char arr[7]={0,8,2,9,37,05,24};
	I2C0_MCS_R =0X03;    // ENABLE  START RUN  BIT
//  
for(i=0;i<7;i++)
	{
		//MASTER_write();             //CHECKING BUSY AND ERROR BIT      
	 while(I2C0_MCS_R&0x01){};
  I2C0_MSA_R = (0x68<<1)&0xFE;    
  I2C0_MSA_R &= ~0x01;             
  I2C0_MDR_R = i&0xFF;         
  I2C0_MCS_R = 0x03;
  while(I2C0_MCS_R&0x01){};
	
  I2C0_MDR_R = arr[i]&0xFF;  
	I2C0_MCS_R = 0x05;
while(I2C0_MCS_R&0x01){};		
	}
  //I2C0_MCS_R =0x05;
	
	//MASTER_write();
}	

void  master_receive()
{

	char data;

for(i=0;i<7;i++)
{
 while(I2C0_MCS_R&0x01){};
  I2C0_MSA_R = (0x68<<1)&0xFE;    
  I2C0_MSA_R &= ~0x01;             
  I2C0_MDR_R = (i)&0xFF;         
  I2C0_MCS_R = 0x07;
  while(I2C0_MCS_R&0x01){};
	// I2C0_MSA_R  &=~(0x01);   
   while(I2C0_MCS_R&0x01){};
    I2C0_MSA_R = (0x68<<1)&0xFE;    
    I2C0_MSA_R |= 0x01;             
    I2C0_MCS_R = 0x07;	// I2C0_MSA_R =0XD1; 
	 
		 
		 //I2C0_MSA_R =0x01;
	//I2C0_MCS_R &=~(0X03);    
//		 I2C0_MCS_R =0X0B; 
		 
		 
 while(I2C0_MCS_R&0x01){};
	 arr1[i]=(I2C0_MDR_R&0xFF);
 arr1[i]=(arr1[i]-6*(arr1[i]>>4));
}
	}
 	
int Enable(void)  
 {
	 
	 GPIO_PORTA_DATA_R |=(0x40);
   KM_delay_ms(5);
GPIO_PORTA_DATA_R &=~(0x40);	 
 return 0;
}
 void high_nibble(char p)
 {
	//GPIO_PORTA_DATA_R &=00;
	//GPIO_PORTA_DATA_R |=(0x80);
	//GPIO_PORTA_DATA_R &=~(0x20);
	GPIO_PORTD_DATA_R &= ~(0x0F);// clear d0 to d3
	GPIO_PORTD_DATA_R |= (p>>4)&0x0F;
  Enable();
 }
 
void low_nibble(char p)
 {
	//GPIO_PORTA_DATA_R &=00;
//	GPIO_PORTA_DATA_R |=(0x80);
	//GPIO_PORTA_DATA_R &=~(0x20);
	GPIO_PORTD_DATA_R &= ~(0x0F);// clear d0 to d3
	GPIO_PORTD_DATA_R |= p&0x0F;  
	Enable();
 }
 void LCD_Write_Data(char x)
 {
	 GPIO_PORTA_DATA_R |=0x80; // Se Data
	 high_nibble(x);
	 low_nibble(x);
 }
 
 void LCD_Write_Cmd(char x)
 {
	 GPIO_PORTA_DATA_R &=~(0x80); // Celar RS
	 high_nibble(x);
	 low_nibble(x);
}	

int main()
{int x;
	
 	
	KM_PLL_Init();	 //INITIALIZAYION OF SYSTEM CLOCK 50HZ
	SysTickTimer_Init(5000000);
	system_clock();
	PORTB_Init();
	PORTD_Init();
	PORTE_Init();

	I2C_Init();
		SYSCTL_Init_lcd();
//	MASTER_write();
//	master_transmission();
		
	KM_delay_ms(100);
//   high_nibble(0x30);
	 
      KM_delay_ms(5);//delay >4.1msec
	    high_nibble(0x30);
	KM_delay_us(120);
	 	    high_nibble(0x30);
	 	    high_nibble(0x20);
  LCD_Write_Cmd(0x0C); // Display ON, Cursor OFF
   LCD_Write_Cmd(0x01); // Display Clear
//  LCD_Write_Cmd(0x80); // Cursor 1st Line	
//		LCD_Write_Data_Str("Time : ");
//    LCD_Write_Cmd(0x87);
	 master_transmitt();	
	while(1)
	{
	master_receive();
//	KM_delay_ms(200);
//	KM_delay_ms(200);
//	KM_delay_ms(200);
//	KM_delay_ms(200);
//	KM_delay_ms(200);
 DHT11_Init();
	DHT11_Request();      // Send Request signal from uP to DHT11
	DHT11_Response();
	
		// Check acknowledgement signal from DHT1
		
		Hum_Hi  = DHT11_Rx_Data();
		Hum_Lo  = DHT11_Rx_Data();
		Temp_Hi = DHT11_Rx_Data();	

		Temp_Lo = DHT11_Rx_Data();
		Checksum = DHT11_Rx_Data();
	
	
	
  //  LCD_Write_Data(Y);
		//LCD_Write_Data(X);
		//KM_delay_ms(200);
		//KM_delay_ms(200);
		//KM_delay_ms(200);
		//KM_delay_ms(200);
		//KM_delay_ms(200);
		 LCD_Write_Cmd(0x80);
		 LCD_Write_Data_Str("t=");
	temp_humd();
	LCD_Write_Cmd(0x82);
	LCD_Write_Data(B);
	 LCD_Write_Data(A);
	 LCD_Write_Data_Str("  D-");
	  
		DHT_RTC();
//		LCD_Write_Data_Str(":");
		LCD_Write_Data(Q);
		LCD_Write_Data(P);
		LCD_Write_Data_Str(":");
		LCD_Write_Data(S);
		LCD_Write_Data(R);
		LCD_Write_Data_Str(":");
//		LCD_Write_Data(H);
//		LCD_Write_Data(G);
LCD_Write_Data_Str("18");

	//LCD_Write_Data_Str("");
	LCD_Write_Cmd(0xC0);
	LCD_Write_Data_Str("h=");
		//LCD_Write_Cmd(0x89);
	LCD_Write_Data(D);
	 LCD_Write_Data(C);
//			KM_delay_ms(200);
//		KM_delay_ms(200);
//		KM_delay_ms(200);
//		KM_delay_ms(200);
//		KM_delay_ms(200);
//		 LCD_Write_Cmd(0xC0);
		// LCD_Write_Data_Str("D:");
		 
		LCD_Write_Data_Str("  T-");
		//LCD_Write_Data_Str("T:");
		LCD_Write_Data(Z);
		LCD_Write_Data(W);
		LCD_Write_Data_Str(":");
		LCD_Write_Data(V);
		LCD_Write_Data(U);
LCD_Write_Data_Str(":");		
	LCD_Write_Data(Y);
	  LCD_Write_Data(X);
	  
		

		//LCD_Write_Data_Str(":");
		//LCD_Write_Data(H);
		//LCD_Write_Data(G);
		
  }
	return 0;
}