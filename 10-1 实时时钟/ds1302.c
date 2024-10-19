#include <regx52.h>

sbit sclk = P3^6;
sbit io = P3^4;
sbit ce = P3^5;

#define SEC    0x80
#define MIN    0x82
#define HOUR   0x84
#define DATE   0x86
#define MONTH  0x88
#define DAY    0x8A
#define YEAR   0x8C
#define WP     0x8E

unsigned char ds_time[] = {24,12,31,23,59,55,2};

void init(){
	ce = 0;
	sclk = 0;
}
//	write(0x80,0x03);
void write(command,DATA){
	unsigned char i;
	ce = 1;
	for(i = 0;i<8;i++){
		io = command&(0x01<<i);
		sclk = 1;
		sclk = 0;
	}
	for(i = 0;i<8;i++){
		io = DATA&(0x01<<i);
		sclk = 1;
		sclk = 0;
	}
	ce = 0;
	
}

unsigned char read(command){
	int i;
	unsigned char DATA = 0x00;
	command |= 0x01;
	ce = 1;
	for(i = 0;i<8;i++){
		io = command&(0x01<<i);
		sclk = 0;
		sclk = 1;
	}
	for(i = 0;i<8;i++){
		sclk = 1;
		sclk = 0;
		if(io) {DATA |= (0x01<<i);}
	}
	ce = 0;
	io = 0;
	return DATA;
}

void ds_setTime(void){
	write(WP,0x00);
	write(YEAR,ds_time[0]/10*16+ds_time[0]%10);
	write(MONTH,ds_time[1]/10*16+ds_time[1]%10);
	write(DATE,ds_time[2]/10*16+ds_time[2]%10);
	write(HOUR,ds_time[3]/10*16+ds_time[3]%10);
	write(MIN,ds_time[4]/10*16+ds_time[4]%10);
	write(SEC,ds_time[5]/10*16+ds_time[5]%10);
	write(DAY,ds_time[6]/10*16+ds_time[6]%10);
}

void ds_readTime(void){
	unsigned char tmp;
	tmp = read(YEAR);
	ds_time[0] = tmp/16*10 + tmp%16;
	tmp = read(MONTH);
	ds_time[1] = tmp/16*10 + tmp%16;
	tmp = read(DATE);
	ds_time[2] = tmp/16*10 + tmp%16;
	tmp = read(HOUR);
	ds_time[3] = tmp/16*10 + tmp%16;
	tmp = read(MIN);
	ds_time[4] = tmp/16*10 + tmp%16;
	tmp = read(SEC);
	ds_time[5] = tmp/16*10 + tmp%16;
	tmp = read(DAY);
	ds_time[6] = tmp/16*10 + tmp%16;
}
