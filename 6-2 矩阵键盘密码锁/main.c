#include <regx52.h>
#include "lcd1602.h"
#include "delay.h"
#include "matrixkey.h"

void main()
{
	int i = 1;
	int res = 0;
	LCD_Init();
	LCD_ShowString(1,1,"PASSWORD:");
	while(1)
	{
		int num = matrixKey();
		
		if(num <= 10 && num>0){ //密码区为1-10
			LCD_ShowNum(2,10,num%10,1);
			res = res*10 + num%10;
			LCD_ShowNum(2,1,res,i);
			i++;
		}
		if(num == 11 && res	== 7890){
			LCD_ShowString(1,14,"OK");
			res = 0;
			i = 1;
		}
		else if(num == 11 && res != 7890){
			LCD_ShowString(1,14,"ERR");
			res = 0;
			i = 1;
		}
		if(num == 12){
			res = 0;
			i = 1;
		}
	}
}