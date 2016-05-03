#ifndef _RHYTHM_H
#define _RHYTHM_H

/******************************节奏设置******************************/
#define timer1_fosc_rate	12							//分频数
#define time1_cell			5							//时间元(ms)
#define FOSC				11059200					//晶振频率
#define level_delay_volume	60							//低水位确认延时(单位s)
#define alarm_delay_volume	2							//故障确认时延(单位s)
#define temp_delay_volume	200							//温度读取时间间隔(单位5ms)
/********************************************************************/

#define		 TH1_TL1			0xffff-((time1_cell*FOSC)/(timer1_fosc_rate*1000))		//装载量
#define		 timer_TH1			TH1_TL1>>8
#define		 timer_TL1			TH1_TL1&0x00ff
#define		 array_passed 		0x01													//时间已到标志
#define		 array_pause		0x02													//时间暂停标志

unsigned char array_status[array_total_volume];											//状态[名字]
unsigned long int array_val[array_total_volume];										//值[名字]

enum line_member
{
	level_delay,
	alarm_delay,
	temperature_delay,
	array_total_volume,
};

void timer1_init();
void RHYTHM_INIT();

#endif