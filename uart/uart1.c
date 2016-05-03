#include <STC12C5A60S2.H>
#include "UART1.H"


/************************************************************************************************************/
#define FOSC 11059200					 //System frequency
#define BAUD 9600						 //UART baudrate
/************************************************************************************************************/



bit uart1_getbyte;
char data_pull;

void uart1_init()
{
	TMOD=TMOD|0x20;						//设定时器1工作方式为方式2(8位自动重装)
    TH1=TL1=-(FOSC/12/32/BAUD); 		//波特率设定
	SCON=0x50;							//串口方式1
	TR1=1;
	ES=1;
    AUXR |= 0x11;
}

void uart1()interrupt 4
{
	uart1_getbyte=1;
	data_pull=SBUF;
	RI=0;
}

void uart1_bytepush(unsigned char x)	
{
	SBUF=x;
	while(!TI);
	TI=0;
}

void uart1_stringpush(char *x)			//字符串输出
{
	ES=0;
	while(*x)
		uart1_bytepush(*x++);
	ES=1;
}