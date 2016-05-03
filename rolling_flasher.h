#ifndef	rolling_flasher_h_
#define	rolling_flasher_h_

#include <STC12C5A60S2.H>
#include "flash.h"

#define sector_empty	0xffff
#define sector_mismatch	0xff0f

enum left_right
{
	left,
	right,
};

enum read_write
{
	read,
	write,
};

enum flash_reg_add
{	
	/*
	供水			0~5	，										//0、2、4开始，1、3、5结束
	加热			6~11，										//6、8、10开始，7、9、11结束
	*/
	temp_upper_limit_low	=24	,								//显示为寄存器值除以10
	temp_upper_limit_high		,
	temp_lower_limit_low		,								//显示为寄存器值除以10
	temp_lower_limit_high		,
	flash_data_count			,								//总共刻录参数28个
};

extern unsigned char	flash_buffer[flash_data_count];			//flash在ram里的备份
extern unsigned char 	active_block_num;						//扇区最后两个字节用于刻录活跃的单元的序号，从0开始
extern unsigned char	active_sector_num;						//储存有用数据的扇区号，从0开始

void FLASHER_INIT();

/*
	读扇区活跃单元序号
	sec：扇区号
	return：活跃单元号*/
unsigned int read_sector_mark(unsigned char sec);

/*
	刻录扇区活跃单元序号
	sec：扇区号
	block：活跃单元号*/
void write_sector_mark(unsigned char sec,unsigned char block);

/*
	写入实际地址add重复读错times次后赋值flash_error，
	或者在规定重读次数前读出正确的值并返回。*/
unsigned char repeat_read(unsigned int add,unsigned char times);

/*
	写入实际地址add重复写值x，times次后赋值flash_error,
	或者在规定重读次数前写入正确的值。*/
void repeat_write(unsigned int add,unsigned char x,unsigned char times);

/*
	flash备份flash_buffer[]里的数据*/
void flash_update();

/*
	读取flash数据
	add:输入flash_reg_add里的偏移量
	data_length：输入所读取的数据的长度1八位，2十六位
	return：flash数据*/
unsigned int flash_read(unsigned char add,unsigned char data_length);

/*
	读取flash数据
	add：输入实际地址
	data_length：输入所读取的数据的长度1八位，2十六位
	return：flash数据*/
unsigned int read_add(unsigned int add,unsigned char data_length);

#endif