#include "ADXL362.h"
#include "spi.h"

//反转字符串  
char *reverse(char *s)  
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
  
/* 
 * 功能：整数转换为字符串 
 * char s[] 的作用是存储整数的每一位 
 */  
char *my_itoa(char n)  
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
	
    return reverse(s);    
}


void ADXL_init()
{
    SPI_init();
		
    ADXL_wirte_oneReg(0x20, 0xfa);
    ADXL_wirte_oneReg(0x21, 0x0);
    ADXL_wirte_oneReg(0x23,0x96);   
    ADXL_wirte_oneReg(0x24,0x00);
    ADXL_wirte_oneReg(0x25,0x1E); 
    ADXL_wirte_oneReg(0x27,0x03);
    ADXL_wirte_oneReg(0x2D,0x02);
}


char get_status()
{
    int i = 0;
    ADXL_wirte_oneReg(0x1F,0x52);   // software reset
    while(i++ < 10000);
    return ADXL_read_oneReg((char)0x0B);
}

void set_self_test()
{
    ADXL_wirte_oneReg(0x2E, 1);
    ADXL_read_oneReg(0x2E);
}

void clr_self_test()
{
    ADXL_wirte_oneReg(0x2E, 0);
}


char get_X()
{
    char addr = ADXL_XDATA;
    char temp = ADXL_read_oneReg(addr);
	return temp;
}

char get_Y()
{
    char temp = ADXL_read_oneRegY(ADXL_YDATA);
	return temp;
} 


char get_Z()
{
    char temp = ADXL_read_oneReg(ADXL_ZDATA);
	return temp;
}  

char get_addr()
{
    ADXL_read_oneReg(0x00);
    ADXL_read_oneReg(0x01);
    ADXL_read_oneReg(0x02);
}
