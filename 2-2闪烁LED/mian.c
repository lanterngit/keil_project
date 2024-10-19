#include <REGX52.H>
#include <INTRINS.H>
void Delay10ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 18;
	j = 235;
	do
	{
		while (--j);
	} while (--i);
}



void main(){
	P2 = 0x55;
	Delay10ms();
	P2 = 0xAA;
	Delay10ms();
}