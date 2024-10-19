#include <REGX52.H>
#include <INTRINS.H>

void Delay100ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 180;
	j = 73;
	do
	{
		while (--j);
	} while (--i);
}

void main(){
	P2 = 0xFE;
	Delay100ms();
	P2 = 0xFD;
	Delay100ms();
	P2 = 0xFB;
	Delay100ms();
	P2 = 0xF7;
	Delay100ms();
	P2 = 0xEF;
	Delay100ms();
	P2 = 0xDF;
	Delay100ms();
	P2 = 0xBF;
	Delay100ms();
	P2 = 0x7F;
	Delay100ms();
}