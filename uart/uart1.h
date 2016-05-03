#ifndef UART1_H_
#define UART1_H_

void uart1_init();

void uart1_bytepush(unsigned char x);		//8位输出  使用前后要配合ES。

void uart1_stringpush(char *x);				//字符串输出如：uart1_stringpush("STC12C5A60S2\r\nUart Test !\r\n")

#endif /* UART1_H_ */
