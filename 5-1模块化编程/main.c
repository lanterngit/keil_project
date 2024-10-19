#include <regx52.h>
#include "delay.h"
#include "display.h"

//void delay(time)		//@11.0592MHz
//{
//	int a;
//	for(a = 0;a<time;a++)
//	{
//		unsigned char i, j;
////		i = 2;
////		j = 199;
////		do
////		{
////			while (--j);
////		} while (--i);
//		i = 43;
//		while (--i);
//	}
//}


void main()
{
	int time = 1;
	display(1,11);
	delay(time);
	display(2,12);
	delay(time);
	display(3,13);
	delay(time);
	display(4,14);
	delay(time);
	display(5,15);
	delay(time);
	display(6,6);
	delay(time);
	display(7,7);
	delay(time);
	display(8,0);
	delay(time);
}