//大端结构
/****************************************************************************************************************
适合少量数据的存储，优点：效率高	缺点：数据量少

		|	第0单元		|	第1单元		|	第2单元		|...|	最后2个字节
flash	|0x0000	0x0001..|1*block_size	|2*block_size	|...|	0x01fe（低位）	0x01ff（高位）	
		|	用户数据	|	用户数据	|	用户数据	|...|	正在使用的（活跃）单元序号
block_num														最高位保留为1，
****************************************************************************************************************/

#include <STC12C5A60S2.H>
#include "rolling_flasher.h"

#define				block_size			32								//每个单元的字节数(约定512字节大小的扇区每个单元的字节数最少32个)

const unsigned char	block_num_limit	=((sector_size-2)/block_size)-1;	//最大单元编号

unsigned char 	active_block_num;										//扇区最后两个字节用于刻录活跃的单元的序号，从0开始
unsigned char	active_sector_num;										//储存有用数据的扇区号，从0开始
unsigned char	flash_buffer[flash_data_count];							//flash在ram里的备份


/*
	用于扇区刻录单元的序号的数值的转换
	x:		输入要转换的数
	lr：	输入方向left：把flash刻录内容转换成数值，right：把数值转换成flash刻录内容
	return:	目标数据,如果输入不符合格式则返回0xffff或0x0000*/
unsigned int ram_flash(unsigned int x,enum left_right lr);

/*
	根据输入的扇区号sec单元号block计算单元首地址*/
unsigned int block_head_locate(unsigned char sec,unsigned char block);

/*
	用于扇区刻录单元的序号的数值的转换
	x:		输入要转换的数
	lr：	输入方向left：把flash刻录内容转换成数值，right：把数值转换成flash刻录内容
	return:	目标数据,如果输入不符合格式则返回0xffff或0x0000*/
unsigned int ram_flash(unsigned int x,enum left_right lr)
{
	unsigned char 	i;
	if(lr==left)
	{
		for(i=1;i<16;i++)
		{
			if(x==(0xffff<<i))
			{
				return i-1;
			}
			if(x==0xffff)
			{
				return sector_empty;											//表示为未被刻录
			}
		}
		return sector_mismatch;													//用于鉴别该扇区是否不符合格式
	}
	else
	{
		if(x<15)
		{
			return 0xffff<<(x+1);
		}
		else
		{
			return 0x0000;														//用于作废扇区
		}
	}
}

/*
	写入实际地址add重复读错times次后赋值flash_error，
	或者在规定重读次数前读出正确的值并返回。*/
unsigned char repeat_read(unsigned int add,unsigned char times)
{
	unsigned char i=0,y;
	do
	{
		if(i>times)
			break;
		y=flashread(add);
		i++;
	}
	while(flash_error&read_error);
	return y;
}

/*
	写入实际地址add重复写值x，times次后赋值flash_error,
	或者在规定重读次数前写入正确的值。*/
void repeat_write(unsigned int add,unsigned char x,unsigned char times)
{
	unsigned char i=0;
	do
	{
		if(i>times)
			break;
		flashwrite(add,x);
		i++;
	}
	while(flash_error&write_error);
}

/*
	读扇区活跃单元序号
	sec：扇区号
	return：活跃单元号*/
unsigned int read_sector_mark(unsigned char sec)
{
	return ram_flash(read_add(((sec+1)*sector_size)-2,2),left);
}

/*
	刻录扇区活跃单元序号
	sec：扇区号
	block：活跃单元号*/
void write_sector_mark(unsigned char sec,unsigned char block)
{
	unsigned int mark=ram_flash(block,right);
	repeat_write((((sec+1)*sector_size)-1),mark>>8,10);
	repeat_write((((sec+1)*sector_size)-2),(mark&0x00ff),10);
}

void FLASHER_INIT()
{
	unsigned char	i;
	unsigned int	count;
	unsigned char 	wd=0;
	FLASH_INIT();
	do											
	{
		for(i=0;i<sector_num;i++)
		{
			count=read_sector_mark(i);
			if(count<=block_num_limit)
			{
				active_block_num=count;											//登记活跃的单元序号
				active_sector_num=i;											//登记有效数据扇区
				return;
			}
		}
		if(wd>2)																//扫描flash两遍如果记录信息丢失
		{
			active_sector_num=0;												//从0号扇区开始								
			active_block_num=0;													//活跃单元指向0号单元	
			for(i=0;i<sector_num;i++)
			{
				flashclear(i);													//清空flash
			}
			break;
		}
		wd++;
	}while(wd<4);
}

/*
	根据输入的扇区号sec单元号block计算单元首地址*/
unsigned int block_head_locate(unsigned char sec,unsigned char block)
{
	return (sec*sector_size)+(block*block_size);
}

/*
	flash备份flash_buffer[]里的数据*/
void flash_update()
{
	bit				miss;														//写入过程出错标记，0出错，1没错														
	unsigned char 	i;
	unsigned char	wd;
	unsigned char	next_block=0xff;											//初值用于校验没意义
	unsigned char	next_sector;
	unsigned int	head_add=block_head_locate(active_sector_num,active_block_num);	//单元首地址
	unsigned int	next_head_add;												//下一个单元首地址
	unsigned char	flash_data;													//目标寄存器现存值
	flash_error&=(~low_power_or_broken);										//消除致命错误
	for(i=0;i<flash_data_count;i++)
	{
		flash_data=repeat_read(head_add+i,10);									//如果连续读取出错只能当最后一次读取值是对的
		if(flash_data!=flash_buffer[i])											//不相同的话判断可否直接写入
		{
			if((flash_data&flash_buffer[i])==flash_buffer[i])
			{
				next_block=active_block_num;									//可以复写
			}
			else
			{
				next_block=active_block_num+1;									//不可以复写
				break;															//不可复写则进入转移单元过程
			}
		}
	}
	if(next_block==0xff)														//数据不需要改动
		return;
	next_sector=active_sector_num;
	wd=0;																		//初始化watchdog
	do
	{
		miss=1;																	//初始化miss为无错
		if(next_block>block_num_limit)
		{
			unsigned char wd1=0;												//初始化watchdog1
			next_block=0;
			do
			{
				next_sector++;
				if(next_sector>=sector_num)
				{
					next_sector=0;
				}
				if(wd1>(sector_num*2))											//搜索整个flash两轮，没有找到可以写入的扇区
				{
					next_sector=0;												//重新定义
					break;
				}
				wd1++;
			}while(read_sector_mark(next_sector)<=block_num_limit);				//遇到没有记录数据的扇区就跳出并擦除
			flashclear(next_sector);											//格式化扇区
		}
		next_head_add=block_head_locate(next_sector,next_block);				//下一个被写入的单元首地址
		for(i=0;i<flash_data_count;i++)
		{
			repeat_write(next_head_add+i,flash_buffer[i],10);					//copy
			if(flash_error&write_error)											//如果写入出错则写进下一个单元
			{
				miss=0;															//标记为出错，需要重新寻找下一个单元写入
				break;
			}
		}
		i=0;
		do																		//标记已用的单元号
		{
			write_sector_mark(next_sector,next_block);							//刻录表示使用过
			if(10<i)
			{
				miss=0;															//标记为出错，需要重新寻找下一个单元写入
				flash_error|=low_power_or_broken;								//判定为低电压或损坏
				break;															//刻录失败超过10次则跳出循环
			}
			i++;
		}while(next_block!=read_sector_mark(next_sector));						//刻录有误则循环刻录
		if(miss==1)																//如果转移顺利
		{
			if(next_sector!=active_sector_num)									//如果涉及扇区间转移
			{
				write_sector_mark(active_sector_num,block_num_limit+1);			//上一个扇区作废
			}
			break;
		}
		wd++;
		if((sector_num*(block_num_limit+1))<=wd)								//切换了整个flash最少1次
		{
			flash_error|=low_power_or_broken;									//判定为低电压或损坏
			return;																//不标记直接报废
		}
		next_block++;
	}while(miss==0);															//写入成功则跳出循环
	active_sector_num=next_sector;
	active_block_num=next_block;
}

/*
	读取flash数据
	add:输入flash_reg_add里的偏移量
	data_length：输入所读取的数据的长度1八位，2十六位
	return：flash数据*/
unsigned int flash_read(unsigned char add,unsigned char data_length)
{
	return read_add(block_head_locate(active_sector_num,active_block_num)+add,data_length);
}

/*
	读取flash数据
	add：输入实际地址
	data_length：输入所读取的数据的长度1八位，2十六位
	return：flash数据*/
unsigned int read_add(unsigned int add,unsigned char data_length)
{
	if(data_length==1)
	{
		return repeat_read(add,10);												//读取flash数据
	}
	else
	{
		return (repeat_read(add+1,10)<<8)|repeat_read(add,10);					//读高低位
	}
}