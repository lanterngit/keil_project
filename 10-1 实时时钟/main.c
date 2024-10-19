#include <regx52.h>
#include "lcd1602.h"
#include "ds1302.h"

void main()
{
	LCD_Init();
	init();
	ds_setTime();
	LCD_ShowString(1,1,"  -  -  ");
	LCD_ShowString(2,1,"  :  :  ");
	while(1)
	{
		ds_readTime();
		LCD_ShowNum(1,1,ds_time[0],2);
		LCD_ShowNum(1,4,ds_time[1],2);
		LCD_ShowNum(1,7,ds_time[2],2);
		LCD_ShowNum(2,1,ds_time[3],2);
		LCD_ShowNum(2,4,ds_time[4],2);
		LCD_ShowNum(2,7,ds_time[5],2);
	}
}