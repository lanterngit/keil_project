#include <regx52.h>
#include "delay.h"

unsigned char key(){
	unsigned char keyNum = 0;
	if(P3_1 == 0){delay(10);while(P3_1 == 0);delay(10);keyNum = 1;}
	if(P3_0 == 0){delay(10);while(P3_0 == 0);delay(10);keyNum = 2;}
	if(P3_2 == 0){delay(10);while(P3_2 == 0);delay(10);keyNum = 3;}
	if(P3_3 == 0){delay(10);while(P3_3 == 0);delay(10);keyNum = 4;}
	return keyNum;
}