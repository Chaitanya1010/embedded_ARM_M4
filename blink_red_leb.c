/* 
		Code to make the RED LEB Blink with some delay
		Pin needed: GPIO PORT F Register 
*/

// Adding the base address and the offset address for each component
// Storing it in Integer pointer as we would be needing to copy 32-bit address(4-Bytes) we'll choose an Integer: 4-Bytes(32bits)

/*** System Control Registers ***/
/*
		Address of RCGCPIO - Used for Enabling the clock to the pins we use
		Enabling clock helps us in using the pin
*/
#define SYSTL_RCGCPIO *((int*) (0x400FE608))
/*
		Address of PRGPIO - A read-only pin, to check the status of the pin we need
		If it is 1 then we can use that pin, else we need to wait
*/	
#define SYSTL_PRGPIO *((int*) (0x400FEA08))


/*** GPIO Registers ***/
/*
		Address of GPIODIR - To provide the Data-Direction (Input or Output)
		Input-0 and Output-1 
*/	
#define GPIOPORT_F_DIR *((int*) (0x40025400))

/*
		Address of GPIODATA - To provide the Data-Input for a given pin in GPIO register
		The Data is stored in here
*/
#define GPIOPORT_F_DATA *((int*) (0x400253FC))

/*
		Address of GPIOAFSEC - To provide Alternative functions for a given pin in GPIO register
		Respective byte of the function we need must be enabled
*/
#define GPIOPORT_F_AFSEC *((int*) (0x40025420))

/*
		Address of GPIOAFSEC - Enable Digital Input for a given pin in GPIO register
		Respective byte of the pin must be enabled
*/
#define GPIOPORT_F_DEN *((int*) (0x4002551C))

/*
		Address of GPIOAFSEC - Enable Analog Input for a given pin in GPIO register
		Respective byte of the pin must be enabled
*/
#define GPIOPORT_F_AMSEL *((int*) (0x40025228))

void PORT_PF_INIT(void)
{
		/*
				Step a: The clock signal must be enabled for the given pin
				We've calculated the RCGCPIO address from it's base and offset address above
				
		*/
		SYSTL_RCGCPIO |= 0x20;
		
		while(!(SYSTL_PRGPIO & 0x20))
		{
			;
		}
		GPIOPORT_F_DIR |=0x02;
		GPIOPORT_F_AFSEC &=(~0x02);
		
		GPIOPORT_F_DEN |=0x02;
		GPIOPORT_F_AMSEL &=(~0x02);	
}

void delay(void)
{
		int i=320000;
		while(i>=0)
			i--;
}

int main(void)
{
		// Step - 1
		// Initialization in PORT_PF_INIT() method
		PORT_PF_INIT();
	
		while(1)
		{
			GPIOPORT_F_DATA ^= 0x02;
			delay();
		}
}
