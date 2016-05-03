#ifndef _adc_h_
#define _adc_h_

void adc_datasaver(unsigned int x);							//adc顺序存储器

unsigned int adc_smoothing(unsigned char x);				//平滑处理器，x为通道号。
						
void adc_init();

#endif
