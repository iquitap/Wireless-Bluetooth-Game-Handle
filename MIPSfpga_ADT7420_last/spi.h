#ifndef _SPI_H_
#define _SPI_H_


#define SPI_BASEADDR 0xB2A00000 //SPI core Addr

#define READ_IO(addr)  (volatile unsigned char *)(addr)
#define WRITE_IO_INT(addr) (volatile unsigned int *)( addr)
#define WRITE_IO_CHAR(addr) (volatile unsigned char *)( addr)
/*****************************************************************************/
/******************* SPI Registers Definitions *******************************/
/*****************************************************************************/

#define SPI_CR_OFFSET 0x60 // SPI control register defalut value :  0x180 = 0110000000b
#define SPI_SR_OFFSET 0x64 // SPI status register
#define SPI_DTR_OFFSET 0x68 //SPI data transmit register
#define SPI_DRR_OFFSET 0x6c //SPI data receive register
#define SPI_SSR_OFFSET 0x70 //SPI Slave select register



char ADXL_read_oneReg(char addr);
void ADXL_wirte_oneReg(char  addr, char send_val);
char ADXL_read_oneRegY(char addr);
void SPI_init();
void SPI_writeOneReg(char  data);

#endif // _SPI_H_