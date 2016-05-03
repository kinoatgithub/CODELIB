#ifndef	flash_h_
#define	flash_h_

#include <STC12C5A60S2.H>

#define test_code			0x55				//用于测试是否低电压操作失败
#define sector_size			0x0200				//扇区地址量
#define sector_num			2					//扇区数量

#define	read_error			0x01
#define write_error 		0x02
#define clear_error			0x04
#define low_power_or_broken	0x08

extern unsigned char flash_error;

void FLASH_INIT();

unsigned int sector_head_locat(unsigned int add);

void clear(unsigned int add);								//扇区擦除是否一定要把该扇区首地址写入

void flashclear(unsigned int add);

unsigned char _read(unsigned int add);

unsigned char flashread(unsigned int add);

void _write(unsigned int add,unsigned char x);

void flashwrite(unsigned int add,unsigned char x);

#endif