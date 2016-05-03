#include "adc.h"
#include <STC12C5A60S2.H>

#define adc_power	0x80
#define adc_flag	0x10
#define adc_start	0x08
#define adc_speed	0x00									//ll:0x00; l:0x20; h:0x40; hh:0x60; 转换速度调整
#define adc_volume	32										//MAX=32

unsigned char adc_datastep=0,adc_channl=0;
unsigned int  adc_table[8*adc_volume];

void adc_datasaver(unsigned int x)
{
	if(adc_datastep==(8*adc_volume)) adc_datastep=0;
	if((adc_channl&0x07)==(adc_datastep&0x07))
	{
		adc_table[adc_datastep]=x;
		adc_datastep++;
	}
}

unsigned int adc_smoothing(unsigned char x)
{
	unsigned char i;
	unsigned int y=0;
	for(i=0;i<=(adc_volume-1);i++) y=y+adc_table[x+i*8];
	y=y/adc_volume;
	return y;
}

void adc_finished() interrupt 5
{
	ADC_CONTR&=!adc_flag;
	adc_datasaver(ADC_RES*4+ADC_RESL);
	adc_channl++;
	ADC_CONTR=adc_power|adc_speed|adc_start|(adc_channl&0x07);
}

void adc_init()
{
	P1ASF=0xff;																//所有P1口设置为模拟量输入口
	ADC_RES=ADC_RESL=0;														//清空转换输出寄存器
	ADC_CONTR=adc_power|adc_speed|adc_start;								//默认从0号通道开始A/D转换
	EADC=1;																	//A/D转换中断允许
}
