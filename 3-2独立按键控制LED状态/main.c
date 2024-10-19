#include <regx52.h>
#include <intrins.h>

void Delay(time)		//@11.0592MHz
{
	int a;
	for(a = 0;a<time;a++){
		unsigned char i, j;
		_nop_();
		i = 2;
		j = 199;
		do
		{
			while (--j);
		} while (--i);
	}
}

void main(){
//	P2_0 = 0;
//	Delay(500);
//	P2_0 = 1;
//	Delay(500);
	if(P3_1 == 0){
		Delay(20);
		while(P3_1 == 0); 
		Delay(20);
		P2_0 = ~P2_0;
	}
}