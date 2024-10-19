#include <regx52.h>
#include "delay.h"
#include "uart.h"


void main()
{
	uartInit();
	while(1)
	{
		
	}
}

void uartRoutine() interrupt 4{
	if(RI == 1){
		P2= SBUF;
		uartSend(SBUF);
		RI = 0;
	}
}