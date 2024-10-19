#include <regx52.h>
#include <intrins.h>

unsigned char table[] = 
{0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x00};
void delay(time)		//@11.0592MHz
{
	int a;
	for(a = 0;a<time;a++)
	{
		unsigned char i, j;
//		i = 2;
//		j = 199;
//		do
//		{
//			while (--j);
//		} while (--i);
		i = 43;
		while (--i);
	}
}

void display(unsigned char address,num)
{
	switch(address)
	{
		case 1: P2_4 = 1; P2_3 = 1; P2_2 = 1;break;
		case 2: P2_4 = 1; P2_3 = 1; P2_2 = 0;break;
		case 3: P2_4 = 1; P2_3 = 0; P2_2 = 1;break;
		case 4: P2_4 = 1; P2_3 = 0; P2_2 = 0;break;
		case 5: P2_4 = 0; P2_3 = 1; P2_2 = 1;break;
		case 6: P2_4 = 0; P2_3 = 1; P2_2 = 0;break;
		case 7: P2_4 = 0; P2_3 = 0; P2_2 = 1;break;
		case 8: P2_4 = 0; P2_3 = 0; P2_2 = 0;break;
	}
	P0 = table[num];
}
void main()
{
	int time = 1;
	display(1,1);
	delay(time);
	display(2,2);
	delay(time);
	display(3,3);
	delay(time);
	display(4,4);
	delay(time);
	display(5,5);
	delay(time);
	display(6,6);
	delay(time);
	display(7,7);
	delay(time);
	display(8,8);
	delay(time);
}