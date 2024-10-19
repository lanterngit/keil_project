#include <regx52.h>
#include <intrins.h>

void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 2;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
}

void DelayAnyms(int time){
	int i;
	for(i = 0;i<time;i++ ){
		Delay1ms();
	}
}
void main(){
	int time = 100;
	P2 = 0xFE;
	DelayAnyms(time);
	P2 = 0xFD;
	DelayAnyms(time);
	P2 = 0xFB;
	DelayAnyms(time);
	P2 = 0xF7;
	DelayAnyms(time);
	P2 = 0xEF;
	DelayAnyms(time);
	P2 = 0xDF;
	DelayAnyms(time);
	P2 = 0xBF;
	DelayAnyms(time);
	P2 = 0x7F;
	DelayAnyms(time);
}