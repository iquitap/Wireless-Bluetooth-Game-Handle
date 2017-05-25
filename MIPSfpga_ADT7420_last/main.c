/*
 * main.c for microAptiv_UP MIPS core running on Nexys4DDR
 * Prints \n\rMIPSfpga\n\r out via UART.
 * Default baudrate 115200,8n1.
 * Also display a shifting 0xf on the LEDs
 */

#include "fpga.h"
//#include "spi.h"
#include "ADXL362.h"
#define inline_assembly()  asm("ori $0, $0, 0x1234")

#define CLK_FREQ 50000000

//低两位为 00 表示是 按键， 10表示是X， 11表示是Y,第二位1表示高字节， 0 表示低字节
#define X_HIGH 0x6 //01 10 
#define X_LOW  0x2 //00 10
#define Y_HIGH 0x7 //01 11
#define Y_LOW  0x3 //00 11
#define BUTTON_HIGH 0x4 //01 00
#define BUTTON_LOW 0x0 //00 00

#define UART_BASE 0xB0401000	//With 1000 offset that axi_uart16550 has
#define rbr		0*4
#define ier		1*4
#define fcr		2*4
#define lcr		3*4
#define mcr		4*4
#define lsr		5*4
#define msr		6*4
#define scr		7*4

#define thr		rbr
#define iir		fcr
#define dll		rbr
#define dlm		ier

#define PWM_BASE 0xB0C00000
#define BUTTON_BASE 0xB6C00000
#define INTC_BASE 0xB0200000
#define INTC_ISR 0x00			/* Interrupt Status Register */
#define INTC_IPR 0x04			/* Interrupt Pending Register */
#define INTC_IER 0x08			/* Interrupt Enable Register */
#define INTC_IAR 0x0c			/* Interrupt Acknowledge Register */
#define INTC_SIE 0x10			/* Set Interrupt Enable bits */
#define INTC_CIE 0x14			/* Clear Interrupt Enable bits */
#define INTC_IVR 0x18			/* Interrupt Vector Register */
#define INTC_MER 0x1c			/* Master Enable Register */

#define INTC_MER_ME (1<<0)
#define INTC_MER_HIE (1<<1)

void delay();
void uart_outbyte(char c);
char uart_inbyte(void);
void uart_print(const char *ptr);

extern char *reverse(char *s);
extern char *my_itoa(char n);

extern void delay_ms(unsigned int ms_count);

extern char get_X();
extern char get_Y();
extern char get_Z();

void xilinx_intc_init();
void intc_disable_interrupts();
void intc_enable_interrupts();

// The following is for ADT7420


/*****************************************************************************/
/********************** Variable Definitions *********************************/
/*****************************************************************************/
volatile int rxData = 0;

char *new_reverse(char *s)  
{  
    char temp;  
    char *p = s;    //p指向s的头部  
    char *q = s;    //q指向s的尾部  
    while(*q)  
        ++q;  
    q--;  
  
    //交换移动指针，直到p和q交叉  
    while(q > p)  
    {  
        temp = *p;  
        *p++ = *q;  
        *q-- = temp;  
    }  
    return s;  
}

char * new_my_itoa(int n)
{
	int i = 0,isNegative = 0;  
    static char s[100];      //必须为static变量，或者是全局变量  
    if((isNegative = n) < 0) //如果是负数，先转为正数  
    {  
        n = -n;  
    }  
    do      //从各位开始变为字符，直到最高位，最后应该反转  
    {  
        s[i++] = n%10 + '0';  
        n = n/10;  
    }while(n > 0);  
  
    if(isNegative < 0)   //如果是负数，补上负号  
    {  
        s[i++] = '-';  
    }  
    s[i] = '\0';    //最后加上字符串结束符  
	
    return new_reverse(s);
}
//------------------
// main()
//------------------
int main() {
	volatile unsigned int pushbutton, count = 0xF;
	volatile unsigned int j = 1;

	*WRITE_IO(UART_BASE + lcr) = 0x00000080; // LCR[7]  is 1
	delay();
	*WRITE_IO(UART_BASE + dll) = 27; // DLL msb. 115200 at 50MHz. Formula is Clk/16/baudrate. From axi_uart manual.
	delay();
	*WRITE_IO(UART_BASE + dlm) = 0 ; // DLL lsb.
	delay();
	*WRITE_IO(UART_BASE + lcr) = 0x00000003; // LCR register. 8n1 parity disabled
	delay();
	*WRITE_IO(UART_BASE + ier) = 0x00000000; // IER register. disable interrupts
	delay();
		
	*WRITE_IO(UART_BASE + ier) = 0x00000001; // IER register. Enables Receiver Line Status and Received Data Interrupts
	delay();

	// initial the axi4 interrupt controller
	xilinx_intc_init( );
	intc_disable_interrupts();
	
	// Begin ADT7420 test
	// Initialize ADT7420 Device
	uart_print("ADXL init\n\r");
	ADXL_init();
	uart_print("ADXL init success\n\r");

	get_addr();
	uart_print("get_addr done\n\r");
	while(1)
    {
    	char temp = get_X();
    	*WRITE_IO(IO_LEDR) = temp;
    	char send_high = (temp & 0xf0) | X_HIGH;
    	char send_low = (temp & 0x0f) << 4 | X_LOW;
    	uart_outbyte(send_high);//send X
    	uart_outbyte(send_low);//send X
    	
    	temp = get_Y();
    	send_high = (temp & 0xf0) | Y_HIGH;
    	send_low = (temp & 0x0f) << 4 | Y_LOW;//send Y
    	uart_outbyte(send_high);//send y
    	uart_outbyte(send_low);//send y

    	unsigned int button = *READ_IO(BUTTON_BASE);
    	temp = ((button & 0x1f) );//get the low 5 bit
    	send_high = (temp & 0xf0) | BUTTON_HIGH;
    	send_low = (temp & 0x0f) << 4 | BUTTON_LOW;
    	uart_outbyte(send_high);
    	uart_outbyte(send_low);//send button*/

    }
	
			
	return 0;
}

void delay() {
	volatile unsigned int j;

	for (j = 0; j < (10000); j++) ;	// delay 
}

void uart_outbyte(char c) {
	*WRITE_IO(UART_BASE + thr) = (unsigned int) c;
	delay( );
}

char uart_inbyte(void) {
	unsigned int RecievedByte;
	
	while(!((*READ_IO(UART_BASE + lsr) & 0x00000001)==0x00000001));

	RecievedByte = *READ_IO(UART_BASE + rbr);

	return (char)RecievedByte;
}

void uart_print(const char *ptr)
{
	while (*ptr) {
		uart_outbyte (*ptr);
		ptr++;
	}
}


void xilinx_intc_init()
{
	/*
	 * Disable all external interrupts until they are
	 * explicity requested.
	 */
	*WRITE_IO(INTC_BASE + INTC_IER) = 0x00000000; 
	delay( );

	/* Acknowledge any pending interrupts just in case. */
	// *WRITE_IO(INTC_BASE + INTC_IAR) = 0xffffffff;
	// delay( );

	// /* Turn on the Master Enable. */
	// *WRITE_IO(INTC_BASE + INTC_MER) = INTC_MER_HIE | INTC_MER_ME;
	// delay( );
	
	// if (!(*READ_IO(INTC_BASE + INTC_MER) & (INTC_MER_HIE | INTC_MER_ME))) {
	// 	uart_print("The MER is not enabled!\n\r");
	// 	*WRITE_IO(INTC_BASE + INTC_MER) = INTC_MER_HIE | INTC_MER_ME;
	// }
	
	/*
	 * Enable INT0-INT7 external interrupts
	 */
	//*WRITE_IO(INTC_BASE + INTC_IER) = 0x000000ff; 
	
	uart_print("The AXI4 interrupt controller is initialized!\n\r");
	
	return;
}


void intc_disable_interrupts()
{
	/*
	 * Disable all external interrupts until they are
	 * explicity requested.
	 */
	*WRITE_IO(INTC_BASE + INTC_IER) = 0x00000000; 
	
	return;
}

void intc_enable_interrupts()
{
	/*
	 * Enable INT0-INT7 external interrupts
	 */
	*WRITE_IO(INTC_BASE + INTC_IER) = 0x000000ff; 
	
	return;
}

void _mips_handle_irq(void* ctx, int reason)
{	
	*WRITE_IO(IO_LEDR) = reason;  // Display 0xFFFF on LEDs to indicate receive data from uart
	delay();
	uart_print("the reason is:");
	uart_print(new_my_itoa(reason));
	uart_print("\n\r");
	uart_print("the addr is:");
	uart_print(new_my_itoa(*(int *)ctx));
	uart_print("\n\r");
	if((*READ_IO(UART_BASE + lsr) & 0x00000001)==0x00000001)
		rxData = *READ_IO(UART_BASE + rbr);
	
	*WRITE_IO(IO_LEDR) = 0x0;
	
	return;
}

