#include <regx52.h>
#include "delay.h"
#include "uart.h"

unsigned char sec = 0;

void main()
{
	uartInit();
	while(1)
	{
		uartSend(sec);
		sec++;
		delay(1000);
	}
}