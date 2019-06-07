
/********* KM BOARD Initalization ( PLL to 50MHZ & SystemTick LIB ) **********/
void KM_Board_Init(void);
void KM_delay_ms(unsigned long delay);
void KM_delay_us(unsigned long delay);

//GPIO
void KM_GPIO_Set(char , int );
void KM_GPIO_Clear(char , int );
void KM_GPIO_Toggle(char , int );
int KM_GPIO_Input(char port, int pin);

//DHT11
void DHT11_Init(void);
void DHT11_Request(void);
int DHT11_Response(void);
int DHT11_Rx_Data(void);

/********* KM 16x32 Monochome LCD LIB **********/
void KM_LCD_Init(void);
void KM_LCD_Write_Cmd( unsigned char  );
void KM_LCD_Write_Data( unsigned char );
void KM_LCD_Write_Str(char *);
char* KM_itoa(int  );

/********* KM ADC LIB **********/
void KM_ADC0_Init(void);
int KM_ADC0_AIN0_Read(void); //LM35
int KM_ADC0_AIN1_Read(void); //Variable Voltage 

/********* KM I2C LIB **********/
// PB2 ----> I2C0_SCL
// PB3 ----> I2C0_SDA

// I2C0 Initalization to 100kHZ
void KM_I2C0_Init(void);

// sends one byte to specified slave
// Returns 0 if successful, nonzero if error
int KM_I2C0_Send1(int , int);

// sends two bytes to specified slave
// Returns 0 if successful, nonzero if error
int KM_I2C0_Send2(int ,int , int);

// receives one byte from specified slave
//  This will work but is probably not what you want to do.
int KM_I2C0_Recv(int);

// receives one byte from specified slave
//  This will work but is probably not what you want to do.
int KM_I2C0_Recv_Slave(int slave);

unsigned char KM_I2C0_ReadRegByte ( unsigned char , unsigned char);

/********* KM UART LIB **********/
// U1Rx (VCP receive) connected to PC4
// U1Tx (VCP transmit) connected to PC5

//------------UART_Init------------
// Initialize the UART for 9600 baud rate (assuming 50 MHz clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void KM_UART1_Init(void);

//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
unsigned char KM_UART1_InChar(void);

//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void KM_UART1_OutChar(unsigned char data);

//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void KM_UART1_OutString(char *pt);

//------------UART_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 32-bit unsigned number
//     valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
unsigned long KM_UART1_InUDec(void);

//-----------------------UART_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void KM_UART1_OutUDec(unsigned long n);

//---------------------UART_InUHex----------------------------------------
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 32-bit unsigned number
// No '$' or '0x' need be entered, just the 1 to 8 hex digits
// It will convert lower case a-f to uppercase A-F
//     and converts to a 16 bit unsigned number
//     value range is 0 to FFFFFFFF
// If you enter a number above FFFFFFFF, it will return an incorrect value
// Backspace will remove last digit typed
unsigned long KM_UART1_InUHex(void);

//--------------------------UART_OutUHex----------------------------
// Output a 32-bit number in unsigned hexadecimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1 to 8 digits with no space before or after
void KM_UART1_OutUHex(unsigned long number);

//------------UART_InString------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed
//    or until max length of the string is reached.
// It echoes each character as it is inputted.
// If a backspace is inputted, the string is modified
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void KM_UART1_InString(char *bufPt, unsigned short max);



// standard ASCII symbols
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F

//UART2 
void UART2_Config(int); // baudrate support 9600 & 115200
void UART2_OutString(char *);
void UART2_OutChar(unsigned char );
void UART2_InString(char *,unsigned short);
unsigned char UART2_InChar(void);


