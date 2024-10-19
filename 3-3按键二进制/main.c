#include <regx52.h>

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
	unsigned char LEDNum = 0;
	while(1)
	{
		if(P3_1==0)
		{
			delay(20);
			while(P3_1 == 0);
			delay(20);
			LEDNum++;
			P2 = ~LEDNum;
		}
	}
}