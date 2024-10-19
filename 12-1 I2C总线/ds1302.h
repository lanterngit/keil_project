#ifndef __DS1302_H__
#define __DS1302_H__

extern unsigned char ds_time[];

void init();
void write(command,DATA);
void ds_readTime(void);
void ds_setTime(void);

unsigned char read(command);

#endif