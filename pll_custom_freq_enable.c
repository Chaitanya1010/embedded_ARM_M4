/*#define SYSTL_RCGCPIO *((int*) (0x400FE608)) 
#define SYSTL_PRGPIO *((int*) (0x400FEA08))
#define GPIOPORT_F_DIR *((int*) (0x40004400))
#define GPIOPORT_F_DATA *((int*) (0x400043FC))
#define GPIOPORT_F_AFSEC *((int*) (0x40004420))
#define GPIOPORT_F_DEN *((int*) (0x4000451C))
#define GPIOPORT_F_AMSEL *((int*) (0x40004228))*/
// these can be added via a company-provided file

#include "km_tm4c123gh6pm.h"
#include "pll.h"

int main(void)
{
		KM_PLL_Init(); // initialization of PLL
}
