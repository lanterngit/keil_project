#include <regx52.h>
#include "lcd1602.h"
#include "delay.h"
#include "key.h"
#include "24C02.h"

unsigned char keyNum;
unsigned int num;

void main()
{
	unsigned char byte;
	LCD_Init();
	LCD_ShowNum(1,1,num,5);
	while(1){
		keyNum = key();
		switch(keyNum){
			case 1:
				num++;
				LCD_ShowNum(1,1,num,5);
				break;
			case 2:
				num--;
				LCD_ShowNum(1,1,num,5);
				break;
			case 3:
				writeByte(0,num%256);
				delay(5);
				writeByte(1,num/256);
				delay(5);
				break;
			case 4:
				num = readByte(0);
				num |= readByte(1)<<8; 
				LCD_ShowNum(1,1,num,5);
				break;
		}
	}
}