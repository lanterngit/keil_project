#include "i2c.h"
#include <regx52.h>
#include "lcd1602.h"
#include "delay.h"

#define AT24c02_address   0xA0

void writeByte(wordAddress,byte){
	start();
	sendByte(AT24c02_address);
	recAck();
	sendByte(wordAddress);
	recAck();
	sendByte(byte);
	recAck();
	end();
	LCD_ShowString(2,1,"WRITE OK!");
	delay(1000);
	LCD_ShowString(2,1,"         ");
}

unsigned char readByte(wordAddress){
	unsigned char byte;
	start();
	sendByte(AT24c02_address);
	recAck();
	sendByte(wordAddress);
	recAck();
	start();
	sendByte(AT24c02_address|0x01);
	recAck();
	byte = recByte();
	sendAck(1);
	end();
	LCD_ShowString(2,1,"READ OK!");
	delay(1000);
	LCD_ShowString(2,1,"         ");
	return byte;
}