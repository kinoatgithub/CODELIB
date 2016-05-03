#ifndef I2C_H_
#define I2C_H_

void i2c_init();

void i2c_start();					

void i2c_stop();

void i2c_respon();

void i2c_pushbyte(unsigned char x);

unsigned char i2c_pullbyte();

#endif /* I2C_H_ */
