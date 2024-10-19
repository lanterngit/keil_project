#include <regx52.h>
#include <intrins.h>
#include "timer0.h"
#include "key.h"
#include "lcd1602.h"

unsigned char keyNum,ledMode;

//void Timer0_Init(){
//	
////	AUXR &= 0x7F;		//定时器时钟12T模式
//	TMOD &= 0xF0;		//设置定时器模式
//	TMOD |= 0x01;		//设置定时器模式
//	TL0 = 0x66;		//设置定时初值
//	TH0 = 0xFC;		//设置定时初值
//	TF0 = 0;		//清除TF0标志
//	TR0 = 1;		//定时器0开始计时
//	ET0 = 1;
//	EA = 1;
//	PT0 = 0;
//}

void main()
{
	LCD_Init();
	P2 = 0xFE;
	Timer0_Init();
	while(1)
	{
		keyNum = key();
		if(keyNum){
			if(keyNum == 1){
				ledMode++;
				if(ledMode>1)ledMode = 0;
			}
			if(keyNum == 2)P2_2 = ~P2_2;
			if(keyNum == 3)P2_3 = ~P2_3;
			if(keyNum == 4)P2_4 = ~P2_4;
		}
	}
}


void Timer0_Routine() interrupt 1{
	static unsigned int T0Count;
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	T0Count++;
	if(T0Count >= 500){
		T0Count = 0;
		if(ledMode == 0)
			P2 = _crol_(P2,1);
		if(ledMode == 1)
			P2 = _cror_(P2,1);
	}
}