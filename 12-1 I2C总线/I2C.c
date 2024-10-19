#include <regx52.h>

sbit sda = P2^0;
sbit scl = P2^1;

void start(){
	sda = 1;
	scl = 1;
	sda = 0;
	scl = 0;
}

void end(){
	sda = 0;
	scl = 1;
	sda = 1;
}

void sendByte(unsigned char byte){
	unsigned char i;
	for(i = 0;i<8;i++){
		sda = byte&(0x80>>i);
		scl = 1;
		scl = 0;
	}
}

unsigned char recByte(){
	unsigned char i,byte = 0x00;
	sda = 1;
	for(i = 0;i<8;i++){
		scl = 1;
		if(sda)byte |= (0x80>>i);
		scl = 0;
	}
	return byte;
	
}

void sendAck(byte){
	sda = byte;
	scl = 1;
	scl = 0;
}

unsigned char recAck(){
	unsigned char ackBit;
	sda = 1;
	scl = 1;
	ackBit = sda;
	scl = 0;
	return ackBit;
}