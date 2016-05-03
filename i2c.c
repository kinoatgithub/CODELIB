/*
max518模拟量输出芯片输出通道地址内容=0x58|(通道号*2);把该内容送出即可表达通道号。
*/

#include "i2c.H"
#include <STC12C5A60S2.H>

sbit sda=P3^6;
sbit scl=P3^7;

void i2c_init()
{
	sda = 1;
	{;;}
	scl = 1;
	{;;}
}

void i2c_start()
{
	sda=1;
	{;;}
	scl=1;
	{;;}
	sda=0;
	{;;}
}

void i2c_stop()
{
	sda=0;
	{;;}
	scl=1;
	{;;}
	sda=1;
	{;;}
}

void i2c_respon()
{
	unsigned char i=0;
	sda=1;
	{;;}
	scl=1;
	{;;}
	while((sda==1)&&(i<=254)) i++;
	scl=0;
	{;;}
}

void i2c_pushbyte(unsigned char x)
{
	unsigned char i;
	scl=0;
	for(i=0;i<=7;i++)
	{
		x=x<<1;
		sda=CY;
		{;;}
		scl=1;
		{;;}
		scl=0;
		{;;}
	}
	sda=1;
	{;;}
}

unsigned char i2c_pullbyte()
{
	unsigned char i=0,k=0;
	scl=0;
	{;;}
	sda=1;
	{;;}
	for (i=0;i<=7;i++)
	{
		scl=1;
		{;;}
		k=(k<<1)|sda;
		scl=0;
		{;;}
	}
	return k;
}