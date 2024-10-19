#include <regx52.h>
#include "delay.h"

sbit rclk = P3^5;
sbit srclk = P3^6;
sbit ser = P3^4;

void write(unsigned char byte){
	int i =0;
	for(i;i<8;i++){
			ser = byte&0x80>>i;
			srclk = 1;
			srclk = 0;
		}
	rclk = 1;
	rclk = 0;
	
}

void display(x,y){
	write(y);
	P0 = ~(0x80>>x);
	delay(1);
	P0 = 0xFF;
}

void main()
{
//	uartInit();
	srclk = 0;
	rclk = 0;
//	display(0,0x01);
	while(1)
	{
		display(0,0x30);
		display(1,0x78);
		display(2,0x7C);
		display(3,0x3E);
		display(4,0x3E);
		display(5,0x7C);
		display(6,0x78);
		display(7,0x30);
		
	}
}