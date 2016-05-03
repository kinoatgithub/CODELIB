#include <STC12C5A60S2.H>
#include "flash.h"

#define	cmd_standby			0
#define cmd_read			1
#define cmd_write			2
#define cmd_clear			3
#define contr_set			0x82
#define	trig_code			0x5a								//触发码

unsigned char flash_error;

void FLASH_INIT()
{
	IAP_CONTR=contr_set;										//允许iap操作且速度设置为<12M的档位
	flash_error=0;
}

unsigned int sector_head_locat(unsigned int add)
{
	unsigned int i;
	i=add&(~(sector_size-1));
	return i;
}

void clear(unsigned int add)									//扇区擦除：把扇区内任一地址写入
{
	IAP_ADDRH=add>>8;
	IAP_ADDRL=add;
	IAP_CMD=cmd_clear;
	IAP_TRIG=trig_code;
	IAP_TRIG=~trig_code;
	IAP_CMD=cmd_standby;										//等待操作完成且防误操作
}

void flashclear(unsigned int add)
{
	unsigned int i;
	unsigned char x;
	unsigned int t=sector_head_locat(add);
	clear(add);
	for(i=t;i<t+sector_size;i++)								//检验是否完全擦除
	{
		x=flashread(i);
		if(!(flash_error&read_error))
		{
			if(x!=0xff)
			{
				flash_error|=clear_error;
				return;
			}
		}
	}
	flash_error&=~clear_error;
}

unsigned char _read(unsigned int add)
{					
	IAP_ADDRH=add>>8;
	IAP_ADDRL=add;
	IAP_CMD=cmd_read;
	IAP_TRIG=trig_code;
	IAP_TRIG=~trig_code;
	IAP_CMD=cmd_standby;
	return IAP_DATA;
}

unsigned char flashread(unsigned int add)
{
	unsigned char i1,i2;
	IAP_DATA=test_code;
	i1=_read(add);
	IAP_DATA=~test_code;
	i2=_read(add);
	if(i1==i2)
		flash_error&=~read_error;
	else
		flash_error|=read_error;
	return i1;
}

void _write(unsigned int add,unsigned char x)
{
	IAP_DATA=x;
	IAP_ADDRH=add>>8;
	IAP_ADDRL=add;
	IAP_CMD=cmd_write;
	IAP_TRIG=trig_code;
	IAP_TRIG=~trig_code;
	IAP_CMD=cmd_standby;
}

void flashwrite(unsigned int add,unsigned char x)
{
	unsigned char i;
	_write(add,x);
	i=flashread(add);
	if(!(flash_error&read_error))
	{
		if(i!=x)
		{
			flash_error|=write_error;
		}
		else
		{
			flash_error&=~write_error;
		}
	}
}