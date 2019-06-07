#include "km_tm4c123gh6pm.h"
#include "kmlib1.h"
#include "pll.h"
//-----------------------
void I2C_Init(void){
  SYSCTL_RCGCI2C_R |= 0x0001;                
  SYSCTL_RCGCGPIO_R |= 0x0002;             
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};    

  GPIO_PORTB_AFSEL_R |= 0x0C;                
  GPIO_PORTB_ODR_R |= 0x08;             		
  GPIO_PORTB_DEN_R |= 0x0C;             		
																						
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00003300;
  GPIO_PORTB_AMSEL_R &= ~0x0C;          		
  I2C0_MCR_R = 0x10;      						
  I2C0_MTPR_R = 24;             					
  
}
//----------------------------
//----------------------------
//----------------------------
int I2C_Recv(int slave){
  int retryCounter = 1;
  do{
    while(I2C0_MCS_R&0x01){};
    I2C0_MSA_R = (slave<<1)&0xFE;    
    I2C0_MSA_R |= 0x01;             
    I2C0_MCS_R = 0x07;
                      
                         
    while(I2C0_MCS_R&0x01){};
    retryCounter = retryCounter + 1;        
  }                                        
  while(((I2C0_MCS_R&0x06) != 0) && (retryCounter <= 5));
  return (I2C0_MDR_R&0xFF);        
}




int I2C_Send1(int slave, int data1){
	
  while(I2C0_MCS_R&0x01){};
  I2C0_MSA_R = (slave<<1)&0xFE;    
  I2C0_MSA_R &= ~0x01;             
  I2C0_MDR_R = data1&0xFF;         
  I2C0_MCS_R = 0x07;
  while(I2C0_MCS_R&0x01){};
                                         
  return (I2C0_MCS_R&0x0E);
}

int I2C0_RTC_SEND(int slave, int data1, int data2){
  while(I2C0_MCS_R&0x01){};
  I2C0_MSA_R = (slave<<1)&0xFE;    
  I2C0_MSA_R &= ~0x01;             
  I2C0_MDR_R = data1&0xFF;         
  I2C0_MCS_R = 0x03;
  while(I2C0_MCS_R&0x01){};
                                          
  if((I2C0_MCS_R&0x0E) != 0){
    I2C0_MCS_R =0x04; 
                                          
    return (I2C0_MCS_R&0x0E);
  }
  I2C0_MDR_R = data2&0xFF;        
  I2C0_MCS_R = 0x05;
                    
  while(I2C0_MCS_R&0x01){};
  return (I2C0_MCS_R&0x0E);
}




unsigned char I2C0_ReadRegByte ( unsigned char address, unsigned char reg )
{
	// point to the register
	I2C_Send1 ( address, reg );

	// return data
	return I2C_Recv( address );
}
//
void rtc_write(void)
{
	int i;
	
	
	I2C0_RTC_SEND(0x68,0x00,0x00);
	I2C0_RTC_SEND(0x68,0x01,0x21);
  I2C0_RTC_SEND(0x68,0x02,0x61);
  I2C0_RTC_SEND(0x68,0x03,0x05);
  I2C0_RTC_SEND(0x68,0x04,0x12);
  I2C0_RTC_SEND(0x68,0x05,0x08);
  I2C0_RTC_SEND(0x68,0x06,0x18);
  I2C0_RTC_SEND(0x68,0x07,0x12);
	 
}
int date=0x00,mon=0x00,year=0x00;
	int hour=0x00,min=0x00,sec=0x00;
int data[7];
int rtc_read(void)
{
	int i;
  sec=I2C0_ReadRegByte(0x68,0x00);
	min=I2C0_ReadRegByte(0x68,0x01);
	hour=I2C0_ReadRegByte(0x68,0x02);
	date=I2C0_ReadRegByte(0x68,0x04);
	mon=I2C0_ReadRegByte(0x68,0x05);
	year=I2C0_ReadRegByte(0x68,0x06);
	
	return 0;

}
//---------
void Lcd_number(unsigned int q)
{
	int i=0;
	unsigned char str[10];
	if(q==0)
		KM_LCD_Write_Data(48);		//display Zero
	while(q>0)
	{
		str[i]=q%10;
		q=q/10;
		i++;
	}
	i--;
	while(i>=0)
	{
		KM_LCD_Write_Data(str[i]+48);
		i--;
	}
}
void SYSTICK_TIMER_INIT()
{
	// Enable Sys Clock
	 NVIC_ST_CTRL_R &= ~(0x01);
	 NVIC_ST_CTRL_R |= 0x04;
	 NVIC_ST_CURRENT_R = 123;
	 NVIC_ST_CTRL_R |=0x01; 
}

//---------
int main(void){  

  KM_PLL_Init();
  SYSTICK_TIMER_INIT();
  I2C_Init();
	KM_LCD_Init();
  KM_LCD_Write_Cmd(0x01);
  //rtc_write();
	while(1)
		{
//----------------------------------//
		rtc_read();
		KM_LCD_Write_Cmd(0x80);
    KM_LCD_Write_Str("TIME:-");
	  Lcd_number((hour>>4)&0x01);
		Lcd_number(hour&0x0F);
	  KM_LCD_Write_Data(':');
    Lcd_number((min>>4)&0x0F);
		Lcd_number(min&0x0F);
	  KM_LCD_Write_Data(':');
		Lcd_number((sec>>4)&0x0F);
	  Lcd_number(sec&0x0F);
		KM_LCD_Write_Cmd(0xC0);  
//--------------------------------//
		KM_LCD_Write_Str("DATE:-");
	  Lcd_number((date>>4)&0x0F);
		Lcd_number(date&0x0F);
	  KM_LCD_Write_Data('/');
    Lcd_number((mon>>4)&0x0F);
		Lcd_number(mon&0x0F);
	  KM_LCD_Write_Data('/');
		Lcd_number(20);
		Lcd_number((year>>4)&0x0F);
	  Lcd_number(year&0x0F);
//-----------------------------------//
	}
	
  
}
void RTC_Out_Date(void)
{
    KM_LCD_Write_Str("DATE:-");
	  Lcd_number((date>>4)&0x0F);
		Lcd_number(date&0x0F);
	  KM_LCD_Write_Data('/');
    Lcd_number((mon>>4)&0x0F);
		Lcd_number(mon&0x0F);
	  KM_LCD_Write_Data('/');
		Lcd_number(20);
		Lcd_number((year>>4)&0x0F);
	  Lcd_number(year&0x0F);
}
void RTC_Out_Time(void)
{
    KM_LCD_Write_Cmd(0xC0);
    KM_LCD_Write_Str("TIME:-");
	  Lcd_number((hour>>4)&0x01);
		Lcd_number(hour&0x0F);
	  KM_LCD_Write_Data(':');
    Lcd_number((min>>4)&0x0F);
		Lcd_number(min&0x0F);
	  KM_LCD_Write_Data(':');
		Lcd_number((sec>>4)&0x0F);
	  Lcd_number(sec&0x0F);
}