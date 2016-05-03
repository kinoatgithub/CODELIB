#ifndef	rtc_h_
#define	rtc_h_
#include <STC12C5A60S2.H>

sbit	rtc_rst		=	P3^3;
sbit	rtc_io		=	P3^4;
sbit	rtc_sclk	=	P3^5;

#define rtc_sec		0x80		//秒：高4十秒，低4秒
#define rtc_min		0x82		//分：高4十分，低4分
#define rtc_hour	0x84		//时：高4十时，低4时
#define rtc_day		0x86		//日：高4十日，低4日
#define rtc_wp		0x8e		//写保护
#define wp_data		0x80		//写保护on
#define rtc_pause	0x80		//计时暂停控制位@rtc_sec

void delay();

/**
 *	rtc写入
 *	add：寄存器地址
 *	x：	 要写入的数据
 */
void rtc_write(unsigned char add,unsigned char x);

/**
 *	rtc读取
 *	add：		寄存器地址
 *	return：	读取的内容
 */
unsigned char rtc_read(unsigned char add);

/**
 *	obj:	时间单位
 *	return：实际数值
 */
unsigned char read_0xrtc(unsigned char obj);

/**
 *	obj：	时间单位
 *	x:		实际数值
 */
void write_0xrtc(unsigned char obj,unsigned char x);

/**
 *	on:	1写保护开，0写保护关
 */
void wp_switch(bit on);

/**时分写入
 *	x:	高8位时，低8位分
 */
void write_rtc_hhmm(unsigned int x);

/**读时分
 *	return：高8时，低8分
 */
unsigned int read_rtc_hhmm();

#endif