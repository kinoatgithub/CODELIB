#include "rhythm.h"
#include <STC12C5A60S2.H>

void timer1_init()
{
	TL1=timer_TL1;
	TH1=timer_TH1;
	TMOD&=0x0f;
	TMOD|=0x10;
	TF1=0;
	ET1=1;
}

void RHYTHM_INIT()
{
	unsigned char i;
	timer1_init();
	for(i=0;i<array_total_volume;i++)
	{
		array_val[i]=0;
		array_status[i]=0;
	}
	TR1=1;	
}

void rhythm()interrupt 3
{
	unsigned char i;
	TR1=0;
	TL1=timer_TL1;
	TH1=timer_TH1;
	TR1=1;
	for(i=0;i<array_total_volume;i++)
	{
		if(array_status[i]==0)
		{
			if(array_val[i]!=0)
			{
				array_val[i]--;
			}
			else
			{
				array_status[i]|=array_passed;
			}
		}
	}
}