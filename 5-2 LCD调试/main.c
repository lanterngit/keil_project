#include <regx52.h>
#include "lcd1602.h"
#include "delay.h"

void main()
{
	int res = 0;
	LCD_Init();
//	LCD_ShowChar(1,1,'A');
//	LCD_ShowString(1,3,"Hello World");
//	LCD_ShowNum(2,1,56,2);
//	LCD_ShowHexNum(2,4,0xAF,2);
//	LCD_ShowBinNum(2,7,0xAF,8);
	while(1)
	{
		res++;
		delay(1000);
		LCD_ShowNum(1,1,res,3);
	}
}