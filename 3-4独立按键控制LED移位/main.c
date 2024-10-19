#include <regx52.h>
#include <stdio.h>

void delay(time)		//@11.0592MHz
{
	int a;
	for(a = 0;a<time;a++)
	{
		unsigned char i, j;
		i = 2;
		j = 199;
		do
		{
			while (--j);
		} while (--i);
	}
}

void main(){
	unsigned char num = 0;
	unsigned char a;
	while(1)
	{
		if(P3_1 == 0)
		{
			delay(20);
			while(P3_1 == 0);
			delay(20);
			if(num>=8) num = 0;
			P2 = ~(0x01<<num);
			num++;
		}
		else if(P3_2 == 0)
		{
			delay(20);
			while(P3_2 == 0);
			delay(20);
			if(num<=0) num = 9;
			a = 9-num;
			P2 = ~(0x80>>a);
			num--;
		}
	}
}