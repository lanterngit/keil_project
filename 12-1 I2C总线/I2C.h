#ifndef __I2C_H__
#define __I2C_H__


void start();
void end();
void sendByte(byte);
unsigned char recByte();
unsigned char recAck();
void sendAck(byte);

#endif