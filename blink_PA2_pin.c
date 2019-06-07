#define SYSTL_RCGCPIO *((int*) (0x400FE608)) 
#define SYSTL_PRGPIO *((int*) (0x400FEA08))
#define GPIOPORT_F_DIR *((int*) (0x40004400))
#define GPIOPORT_F_DATA *((int*) (0x400043FC))
#define GPIOPORT_F_AFSEC *((int*) (0x40004420))
#define GPIOPORT_F_DEN *((int*) (0x4000451C))
#define GPIOPORT_F_AMSEL *((int*) (0x40004228))

void PORT_PF_INIT(void)
{
		SYSTL_RCGCPIO |= 0x01;
		
		while(!(SYSTL_PRGPIO & 0x01))
		{
			;
		}
		GPIOPORT_F_DIR |=0x04;
		GPIOPORT_F_AFSEC &=(~0x04);
		
		GPIOPORT_F_DEN |=0x04;
		GPIOPORT_F_AMSEL &=(~0x04);	
}

void delay(void)
{
		int i=320000;
		while(i>=0)
			i--;
}

int main(void)
{
		PORT_PF_INIT();
		while(1)
		{
			GPIOPORT_F_DATA ^= 0x04;
			delay();
		}
}
