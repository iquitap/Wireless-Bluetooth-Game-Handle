#include "spi.h"

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

char * _reverse(char *s)  
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

char * _my_itoa(int n)
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
	
    return _reverse(s);
}

void _uart_outbyte(char c) {
	*WRITE_IO_INT(UART_BASE + thr) = (unsigned int) c;
	delay( );
}



void _uart_print(const char *ptr)
{
	while (*ptr) {
		_uart_outbyte (*ptr);
		ptr++;
	}
}

void new_delay() {
	volatile unsigned int j;

	for (j = 0; j < (100); j++) ;	// new_delay 
}

void little_delay() {
	volatile unsigned int j;

	for (j = 0; j < (50); j++) ;	// new_delay 
}

char ADXL_read_oneReg(char addr)
{
	*WRITE_IO_INT(SPI_BASEADDR + SPI_SSR_OFFSET) = 0xffffffff;

	char temp;
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x186;
	little_delay();
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_DTR_OFFSET) = 0x0B;
	little_delay();
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_DTR_OFFSET) = 0x08;
	little_delay();
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_DTR_OFFSET) = 0x00;
	*WRITE_IO_INT(SPI_BASEADDR + SPI_SSR_OFFSET) = 0xfffffffe;
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x086;
	little_delay();
	*WRITE_IO_INT(SPI_BASEADDR + SPI_SSR_OFFSET) = 0xffffffff;
	little_delay();
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x186;
	while(*READ_IO(SPI_BASEADDR + SPI_SR_OFFSET) & 1)
	{
		/*_uart_print("the status is :");
		_uart_print(_my_itoa(*READ_IO(SPI_BASEADDR + SPI_SR_OFFSET)));
		_uart_print("\n\r");*/
	}
	while((*READ_IO(SPI_BASEADDR + SPI_SR_OFFSET) & 1) == 0)
	{
		/*_uart_print("before the status is :");
		_uart_print(_my_itoa(*READ_IO(SPI_BASEADDR + SPI_SR_OFFSET)));
		_uart_print("\n\r");
		_uart_print("get data successful\n\r");*/
		temp = *READ_IO(SPI_BASEADDR + SPI_DRR_OFFSET);
		/*_uart_print("temp is :");
		_uart_print(_my_itoa(temp));
		_uart_print("\n\r");
		_uart_print("after the status is :");
		_uart_print(_my_itoa(*READ_IO(SPI_BASEADDR + SPI_SR_OFFSET)));
		_uart_print("\n\r");*/
		
	}
	//_uart_print("quit read\n\r");
	return temp;
}

char ADXL_read_oneRegY(char addr)
{
	*WRITE_IO_INT(SPI_BASEADDR + SPI_SSR_OFFSET) = 0xffffffff;
	char temp;	
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x186;
	little_delay();
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_DTR_OFFSET) = 0x0B;
	little_delay();
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_DTR_OFFSET) = 0x09;
	little_delay();
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_DTR_OFFSET) = 0x00;
	*WRITE_IO_INT(SPI_BASEADDR + SPI_SSR_OFFSET) = 0xfffffffe;
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x086;
	little_delay();
	*WRITE_IO_INT(SPI_BASEADDR + SPI_SSR_OFFSET) = 0xffffffff;
	little_delay();
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x186;
	while(*READ_IO(SPI_BASEADDR + SPI_SR_OFFSET) & 1)
	{
	}
	while((*READ_IO(SPI_BASEADDR + SPI_SR_OFFSET) & 1) == 0)
	{
		temp = *READ_IO(SPI_BASEADDR + SPI_DRR_OFFSET);
		
	}
	return temp;
}


void ADXL_wirte_oneReg(char  addr, char send_val)
{

	/*SPI_writeOneReg(0x0A);
	SPI_writeOneReg(addr);
	SPI_writeOneReg(send_val);*/
	*WRITE_IO_INT(SPI_BASEADDR + SPI_SSR_OFFSET) = 0xffffffff;
	//_uart_print("enter write\n\r");
	//*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x1E6;
	//_uart_print("addr is");
	//_uart_print(_my_itoa(addr));
	//_uart_print("\n\r");
	new_delay();
	new_delay();
	new_delay();
	new_delay();
	//_uart_print("the SPI_CR is :");
	int temp = *WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET);
	_uart_print(_my_itoa(temp));
	_uart_print("\n\r");
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x186;
	new_delay();
	//_uart_print("the SPI_CR is :");
	temp = *WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET);
	_uart_print(_my_itoa(temp));
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_DTR_OFFSET) = 0x0A;
	new_delay();
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_DTR_OFFSET) = 0x20;
	new_delay();
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_DTR_OFFSET) = 0x55;
	new_delay();
	//_uart_print("the status is :");
	_uart_print(_my_itoa(*READ_IO(SPI_BASEADDR + SPI_SR_OFFSET)));
	//_uart_print("\n\r");
	//*WRITE_IO_INT(SPI_BASEADDR + SPI_DTR_OFFSET) = 0x0A << 16 | addr << 8 | send_val;
	new_delay();
	*WRITE_IO_INT(SPI_BASEADDR + SPI_SSR_OFFSET) = 0xfffffffe;
	/*new_delay();
	temp = *WRITE_IO_INT(SPI_BASEADDR + SPI_SSR_OFFSET);
	_uart_print(" the ssr is :");
	_uart_print(_my_itoa(temp));
	_uart_print("\n\r");*/
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x0E6;
	new_delay();
	temp = *WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET);
	//_uart_print("enable the status is :");
	_uart_print(_my_itoa(temp));
	_uart_print("\n\r");
	*WRITE_IO_INT(SPI_BASEADDR + SPI_SSR_OFFSET) = 0xffffffff;
	new_delay();
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x186;
	//_uart_print("the status is :");
	_uart_print(_my_itoa(*READ_IO(SPI_BASEADDR + SPI_SR_OFFSET)));
	//_uart_print("\n\r");
	//_uart_print("quit write\n\r");
}

void SPI_init()
{
	/*_uart_print("the init SPI_CR is :");
	int temp = *WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET);
	_uart_print(_my_itoa(temp));
	_uart_print("\n\r");*/

	*WRITE_IO_INT(SPI_BASEADDR + 0x40) = 0x0000000a;
	/**WRITE_IO_CHAR(SPI_BASEADDR + SPI_SSR_OFFSET) = 1;//cs to high
	new_delay();

	/*temp |= 0x066;
	_uart_print("the itemp is :");
	_uart_print(_my_itoa(temp));
	_uart_print("\n\r");*/

	/**WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x1E6;*/
	new_delay();
	//_uart_print("first  SPI_CR is :");
	//int temp = *WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET);
	//_uart_print(_my_itoa(temp));
	//_uart_print("\n\r");
}


void SPI_writeOneReg(char  data)
{
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_SSR_OFFSET) = 1;
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x1E6;
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_DTR_OFFSET) = data;
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_SSR_OFFSET) = 0;
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x0E6;
	*WRITE_IO_CHAR(SPI_BASEADDR + SPI_SSR_OFFSET) = 1;
	*WRITE_IO_INT(SPI_BASEADDR + SPI_CR_OFFSET) = 0x1E6;
}