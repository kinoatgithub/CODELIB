#include "i2c.H"
#include "dac.H"

void dac_output(unsigned char x,unsigned char ch0,unsigned char ch1)
{
	i2c_start();
	i2c_pushbyte((x*2)|0x58);
	i2c_respon();
	i2c_pushbyte(0x00);
	i2c_respon();
	i2c_pushbyte(ch0);
	i2c_respon();
	i2c_pushbyte(0x01);
	i2c_respon();
	i2c_pushbyte(ch1);
	i2c_respon();
	i2c_stop();
}

void dac_init()
{
	dac_output(0,0,0);
	dac_output(1,0,0);
}
