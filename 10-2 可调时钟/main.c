#include <regx52.h>
#include "lcd1602.h"
#include "delay.h"
#include "ds1302.h"
#include "key.h"
#include "timer0.h"

unsigned char keyNum,mode,timeSelect,timeFlash;

void timeShow(){
	ds_readTime();
	LCD_ShowNum(1,1,ds_time[0],2);
	LCD_ShowNum(1,4,ds_time[1],2);
	LCD_ShowNum(1,7,ds_time[2],2);
	LCD_ShowNum(2,1,ds_time[3],2);
	LCD_ShowNum(2,4,ds_time[4],2);
	LCD_ShowNum(2,7,ds_time[5],2);
}

void timeSet(){
	if(keyNum == 2){
		timeSelect++;
		timeSelect%=6;
	}
	if(keyNum == 3){
		ds_time[timeSelect]++;
		if(ds_time[0]>99){ds_time[0]=0;}//年越界判断
		if(ds_time[1]>12){ds_time[1]=1;}//月越界判断
		if( ds_time[1]==1 || ds_time[1]==3 || ds_time[1]==5 || ds_time[1]==7 || 
			ds_time[1]==8 || ds_time[1]==10 || ds_time[1]==12)//日越界判断
		{
			if(ds_time[2]>31){ds_time[2]=1;}//大月
		}
		else if(ds_time[1]==4 || ds_time[1]==6 || ds_time[1]==9 || ds_time[1]==11)
		{
			if(ds_time[2]>30){ds_time[2]=1;}//小月
		}
		else if(ds_time[1]==2)
		{
			if(ds_time[0]%4==0)
			{
				if(ds_time[2]>29){ds_time[2]=1;}//闰年2月
			}
			else
			{
				if(ds_time[2]>28){ds_time[2]=1;}//平年2月
			}
		}
		if(ds_time[3]>23){ds_time[3]=0;}//时越界判断
		if(ds_time[4]>59){ds_time[4]=0;}//分越界判断
		if(ds_time[5]>59){ds_time[5]=0;}//秒越界判断
	}
	if(keyNum == 4){
		ds_time[timeSelect]--;
		if(ds_time[0]<0){ds_time[0]=99;}//年越界判断
		if(ds_time[1]<1){ds_time[1]=12;}//月越界判断
		if( ds_time[1]==1 || ds_time[1]==3 || ds_time[1]==5 || ds_time[1]==7 || 
			ds_time[1]==8 || ds_time[1]==10 || ds_time[1]==12)//日越界判断
		{
			if(ds_time[2]<1){ds_time[2]=31;}//大月
			if(ds_time[2]>31){ds_time[2]=1;}
		}
		else if(ds_time[1]==4 || ds_time[1]==6 || ds_time[1]==9 || ds_time[1]==11)
		{
			if(ds_time[2]<1){ds_time[2]=30;}//小月
			if(ds_time[2]>30){ds_time[2]=1;}
		}
		else if(ds_time[1]==2)
		{
			if(ds_time[0]%4==0)
			{
				if(ds_time[2]<1){ds_time[2]=29;}//闰年2月
				if(ds_time[2]>29){ds_time[2]=1;}
			}
			else
			{
				if(ds_time[2]<1){ds_time[2]=28;}//平年2月
				if(ds_time[2]>28){ds_time[2]=1;}
			}
		}
		if(ds_time[3]<0){ds_time[3]=23;}//时越界判断
		if(ds_time[4]<0){ds_time[4]=59;}//分越界判断
		if(ds_time[5]<0){ds_time[5]=59;}//秒越界判断
	}
	if(timeSelect==0 && timeFlash==1){LCD_ShowString(1,1,"  ");}
	else {LCD_ShowNum(1,1,ds_time[0],2);}
	if(timeSelect==1 && timeFlash==1){LCD_ShowString(1,4,"  ");}
	else {LCD_ShowNum(1,4,ds_time[1],2);}
	if(timeSelect==2 && timeFlash==1){LCD_ShowString(1,7,"  ");}
	else {LCD_ShowNum(1,7,ds_time[2],2);}
	if(timeSelect==3 && timeFlash==1){LCD_ShowString(2,1,"  ");}
	else {LCD_ShowNum(2,1,ds_time[3],2);}
	if(timeSelect==4 && timeFlash==1){LCD_ShowString(2,4,"  ");}
	else {LCD_ShowNum(2,4,ds_time[4],2);}
	if(timeSelect==5 && timeFlash==1){LCD_ShowString(2,7,"  ");}
	else {LCD_ShowNum(2,7,ds_time[5],2);}
}

void main()
{
	LCD_Init();
	init();
	Timer0_Init();
	ds_setTime();
	LCD_ShowString(1,1,"  -  -  ");
	LCD_ShowString(2,1,"  :  :  ");
	while(1)
	{
		keyNum = key();
		if(keyNum == 1){
			if(mode == 0) mode = 1;
			else if(mode == 1) mode = 0;
		}
		switch(mode){
			case 0: timeShow();break;
			case 1: timeSet();break;
		}
	}
}

void timer0_routine() interrupt 1{
	static unsigned int T0Count;
	TL0 = 0x18;
	TH0 = 0xFC;
	T0Count++;
	if(T0Count>=500){
		T0Count = 0;
		timeFlash != timeFlash;
	}
}