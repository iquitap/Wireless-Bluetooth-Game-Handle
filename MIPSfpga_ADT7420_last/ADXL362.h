#ifndef _ADXL362_H_
#define _ADXL362_H_

#define ADXL_XDATA_LOW 0x0E
#define ADXL_XDATA_HIGH 0x0F
#define ADXL_YDATA_LOW 0x10
#define ADXL_YDATA_HIGH 0x11
#define ADXL_ZDATA_LOW 0x12
#define ADXL_ZDATA_HIGH 0x13

#define ADXL_XDATA 0x08
#define ADXL_YDATA 0x09
#define ADXL_ZDATA 0x0A
#define ADXL_STATUS 0x0B

#define ADXL_STATUS_DEVAL 0x80

char *reverse(char *s);
char *my_itoa(char n);
void ADXL_init();

char get_status();
void set_self_test();
void clr_self_test();

char get_X();
char get_Y();
char get_Z();
char get_addr();
#endif //_ADXL362_H_