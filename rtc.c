#include "rtc.h"

/**
 *	rtc写入
 *	add：寄存器地址
 *	x：	 要写入的bcd码
 */
void rtc_write(unsigned char add,unsigned char x)
{
	unsigned char i,j;
	rtc_rst=0;
	rtc_sclk=0;
	j=add;
	rtc_rst=1;
	for(i=0;i<8;i++)
	{
		if(j&0x01)
			rtc_io=1;
		else
			rtc_io=0;
		rtc_sclk=1;
		j>>=1;
		rtc_sclk=0;
	}
	j=x;
	for(i=0;i<8;i++)
	{
		if(j&0x01)
			rtc_io=1;
		else
			rtc_io=0;
		rtc_sclk=1;
		j>>=1;
		rtc_sclk=0;
	}
	rtc_rst=0;
	rtc_io=1;								//省电
}

/**
 *	rtc读取
 *	add：		寄存器地址
 *	return：	读取的bcd码
 */
unsigned char rtc_read(unsigned char add)
{
	unsigned char i,j;
	rtc_rst=0;
	rtc_sclk=0;
	j=add+1;
	rtc_rst=1;
	for(i=0;i<8;i++)
	{
		rtc_io=(bit)(j&0x01);
		rtc_sclk=1;
		j>>=1;
		rtc_sclk=0;
	}
	rtc_io=1;
	for(i=0;i<8;i++)
	{
		j>>=1;	
		if(rtc_io)
			j|=0x80;
		rtc_sclk=1;
		rtc_sclk=0;
	}
	rtc_rst=0;
	return j;
}

/**
 *	obj:	时间单位
 *	return：bcd码
 */
unsigned char read_0xrtc(unsigned char obj)
{
	return 0x7f&rtc_read(obj);
}

/**
 *	obj：	时间单位
 *	x:		bcd码
 */
void write_0xrtc(unsigned char obj,unsigned char x)
{
	rtc_write(obj,x);
}

/**
 *	on:	1写保护开，0写保护关
 */
void wp_switch(bit on)
{
	if(on)
		write_0xrtc(rtc_wp,wp_data);
	else
		write_0xrtc(rtc_wp,0);
}

/**时分写入
 *	x:	高8位bcd时，低8位bcd分
 */
void write_rtc_hhmm(unsigned int x)
{
	unsigned char i;
	i=x>>8;
	wp_switch(0);
	write_0xrtc(rtc_hour,i);
	i=x;
	write_0xrtc(rtc_min,i);
	wp_switch(1);
}

/**读时分
 *	return：高8bcd时，低8bcd分
 */
unsigned int read_rtc_hhmm()
{
	unsigned int i;
	i=(read_0xrtc(rtc_hour)<<8)|read_0xrtc(rtc_min);
	return i;
}

/**时间暂停/开始
 *	stop 是否停止1：是，0否
 */
void rtc_time_pause(bit stop)
{
	write_0xrtc(rtc_min,read_0xrtc(rtc_sec)|((rtc_pause)*stop));
}