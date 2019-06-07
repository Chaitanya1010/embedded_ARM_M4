#include "km_tm4c123gh6pm.h"
#include "pll.h"
#include "kmlib1.h"
/* 
	Actual LCD 16x2 pins
	16 coloumns and 2 rows -> 4 data-pins(from 8 data-pins) + 3 command pins
	3 command pins:
	
	RS : `PA7` (command/data)
		0 -> command to LCD
		1 -> data to LCD
		
	EN : `PA6` (Enables the input to LCD)
		initially : 0
		1 -> 10ms -> 0
		Input is taken by the LCD
		After command/data input, this must be triggered
	
	RW : `PA5` (Write mode)
		0 -> Write to LCD(max we use this, as we write data to LCD)
		1 -> Read from LCD
	
	Command Mode: (RW=0;RS=0)
		01 - clear display
		0E - cursor blinking ,etc
				1. These commands are given inside the data registers
				2. Upper Nibble is sent (Starting 4 bits are sent first, by x>>4) 
				3. After every command we use enable(): EN set -> wait for 10ms -> EN disable
				4. Lower Nibble is sent (Ending 4 bits are sent at last)
	
	Data Mode: (RW=0;RS=1)
	
	D0 : `PD0`
	D1 : `PD1`
	D2 : `PD2`
	D3 : `PD3`
	
	Steps:
		1. RW=0
		2. RS=1
		3. Upper Nibble to D0-D3(using the shift operator x>>4)
		4. enable() En set -> 10ms -> En unset
		5. Send Lower Nibble to D0-D3
		6. enable() En set -> 10ms -> En unset
*/
void SYSTICK_TIMER_INIT()
{
	// Enable Sys Clock
	 NVIC_ST_CTRL_R |= 0x04;
	
	// Disable the Enable bit
	 NVIC_ST_CTRL_R &= ~(0x01);
	
	//Clear the data inside STCURRENT and COUNT flag by writing data into STCURRENT
	// NVIC_ST_CURRENT_R = 123; // Since it is a write-clear register, this earases the count flag and this register
	
	// Add the value into STRELOAD
	 // NVIC_ST_RELOAD_R = 50; //1us

	// Enable the SYS clock
	 NVIC_ST_CTRL_R |=0x01; 
}
/*
void delay(int y)
{
	  int count = 50*y;
		NVIC_ST_CURRENT_R = 3214;
		NVIC_ST_RELOAD_R = count;
				
		while(!(NVIC_ST_CTRL_R&0x10000))
		{
			;
		}
				
		NVIC_ST_RELOAD_R =0;
}*/

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

void cursor_blink()
{
		write_cmd(0xC0);
}

int main(void)
{ 
		// Initialize PLL for the 
		GPIO_PORTD_PORTA_ENABLE();
	KM_PLL_Init();
			SYSTICK_TIMER_INIT();
	initialize_LCD_for_4bit_input();	
	/*write_data(' ');
	write_data(' ');
	write_data(' ');
	write_data(' ');
	write_data(' ');
	write_data(' ');
		
	write_data('I');
	write_data('A');
	write_data('M');
		write_data('N');
	write_cmd(0xC0);
	write_data(' ');
	write_data(' ');
	write_data(' ');
	write_data(' ');
		write_data(' ');
	
		write_data('I');
		write_data('R');
		write_data('O');
		write_data('N');
		write_data('M');
		write_data('A');
		write_data('N');
	*/
	
	
	
}
