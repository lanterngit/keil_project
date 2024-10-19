#include <regx52.h>
#include "delay.h"

int matrixKey(){
	int keyNum = 0;
	
	P1 = 0xFF;
	P1_3 = 0;
	if(P1_7 == 0){delay(10);while(P1_7 == 0);delay(10);keyNum = 1;}
	if(P1_6 == 0){delay(10);while(P1_6 == 0);delay(10);keyNum = 5;}
	if(P1_5 == 0){delay(10);while(P1_5 == 0);delay(10);keyNum = 9;}
	if(P1_4 == 0){delay(10);while(P1_4 == 0);delay(10);keyNum = 13;}
	
	P1_2 = 0;
	if(P1_7 == 0){delay(10);while(P1_7 == 0);delay(10);keyNum = 2;}
	if(P1_6 == 0){delay(10);while(P1_6 == 0);delay(10);keyNum = 6;}
	if(P1_5 == 0){delay(10);while(P1_5 == 0);delay(10);keyNum = 10;}
	if(P1_4 == 0){delay(10);while(P1_4 == 0);delay(10);keyNum = 14;}
	
	P1_1 = 0;
	if(P1_7 == 0){delay(10);while(P1_7 == 0);delay(10);keyNum = 3;}
	if(P1_6 == 0){delay(10);while(P1_6 == 0);delay(10);keyNum = 7;}
	if(P1_5 == 0){delay(10);while(P1_5 == 0);delay(10);keyNum = 11;}
	if(P1_4 == 0){delay(10);while(P1_4 == 0);delay(10);keyNum = 15;}
	
	P1_0 = 0;
	if(P1_7 == 0){delay(10);while(P1_7 == 0);delay(10);keyNum = 4;}
	if(P1_6 == 0){delay(10);while(P1_6 == 0);delay(10);keyNum = 8;}
	if(P1_5 == 0){delay(10);while(P1_5 == 0);delay(10);keyNum = 12;}
	if(P1_4 == 0){delay(10);while(P1_4 == 0);delay(10);keyNum = 16;}
	
	return keyNum;
}