/**
    *************************************************************
		* @file     adc_sampling.c
    * @brief    main program
		*************************************************************
		*            ˵����
		*
    * ��ѹ������������
    * �Ե�ѹ�͵������в����������������ݽ�������
		* TMR3Ϊ������ʱ��������Ƶ��Ϊ2000HZ
		* ADCʱ��Ϊ4MHZ  ����ʱ��7.5��ADCCLK,
		* ������ʽ7·ADC��������,ÿ�����78������
		* ͨ��DMA�����ݷ��͵���ʾ�����С�
		* �жϵ�ѹ�͵����Ƿ�ϸ�,
		*/
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "at32f421_wk_config.h"
#include "at32f421_conf.h"
#include "at32f421_int.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "adc_sampling.h"
#include "flash.h"

#define S1_Upward_In      GPIO_PINS_14//PC14
#define S2_Downward_In    GPIO_PINS_15//PC15
#define S3_Mode_In        GPIO_PINS_0//PF0

const char SMG_Duan[21] = {0xD7,0x14,0xCD,0x5D,0x1E,0x5B,0xDB,0x15,0xDF,0x5F,0x8F,0xD6,0xC2,0xD8,0x8B,0x98,0xCB,0xDC,0x9F,0xC2,0x00};        //���������λ�Ĵ���
//const char SMG_Duane[10] = {0x8F,0xD6,0xC2,0xD8,0x8B,0x98,0xCB,0xDC,0x9F,0xC2};       //P,U,C,o,F,n,E,d,A,L
extern char table_array[];  //��ʾ��������
extern u16 data_int[];   //����������ʱ�Ĵ������飬�����ݴ�������ݡ�
extern const uint16_t FFT_LENGTH;  // ��������
extern const float Fs;    // ����Ƶ��
extern uint16_t User_Parameter[18];  //�û��趨����
extern const uint32_t TEST_FLASH_ADDRESS_START;
extern const uint16_t TEST_BUFEER_SIZE;

//extern uint16_t ADC_DMA_data[];  //ADC�������ݼĴ�������
extern uint16_t ADC_Channel_0[];   //ADC�������ݷ���ͨ��1���顣
//extern uint16_t ADC_Channel_2[];   //ADC�������ݷ���ͨ��2���顣
//extern uint16_t ADC_Channel_3[];   //ADC�������ݷ���ͨ��3���顣
extern float fft_outputbuf[]; //ת��������

extern float FFT_UADC_DATA;//ADC������Чֵ����
extern float FFT_IADC_DATA;  //ת����ĵ�����Чֵ
extern float FFT_FREQ_DATA;  //ת�����Ƶ��ֵ
extern float FFT_PHASE_DATA;  //ת����ĽǶ�����
	
extern flag_status pins;

uint16_t max1,max2;


/**
  *������������4λ����
  */
/*uint16_t* right_shift(uint16_t* right_data,uint16_t right_size)
{
	 uint16_t i;
	 for(i=0;i<right_size;i++)
	 {
	    right_data[i] = right_data[i] >> 4;
	 }
	 return rig
}*/
/**
    * ����OR������ת������
    * ��int������ת��Ϊ���������飬��ʾ����ֵ
		* �������ݷ�Ϊʵ�����鲿
		* ʵ��Ϊ������ʵ������
		* �鲿Ϊ0
    * �������ݱ�����data_int�����ڡ�
    * ת�������ݱ�����fft_inputbuf������.  
		*/
/*void convertIntFloat(uint16_t* data_int, float* fft_inputbuf)
{
	  uint16_t i;
    for (i = 0; i < FFT_LENGTH; i++) 
	  {
        //fft_inputbuf[2*i] = (short)data_int[i]*3.3f/4095.0f;//ADCʵ��
			  fft_inputbuf[2*i] = (1.2f / ADC_Channel_0[i]) * data_int[i];//ADCʵ��
			  fft_inputbuf[2*i+1] = 0.0f;//�鲿Ϊ0
    }
}*/


/**
    * ��ѹ��Ƶ�ʼ������
    * ��ȡƵ�����ݣ��ҵ�����0�����������ݣ��ҳ���Ӧ��Ƶ�ʺͷ��ȡ�
    * Ƶ��������fft_outputbuf�����У�
    * ����0�������Ϊֱ����������������
    * ������0�������Ϊ��ӦƵ�ʵ�г������������������Ϊ��Ҫ����������
    * ����0-FFT_LENGTH/2Ϊ��Ƶ�ʣ�����FFT_LENGTH/2-FFT_LENGTHΪ��Ƶ��
    * ����������õ���Ӧ�����ţ�
    * Ƶ�� = ����Ƶ�� / ��������  *  ������
    * ��ֵ = ����[���] * 2 / ��������
    * ��Чֵ = ��ֵ / 1.414(����2��
		*/
/*void frequency_count(void)
{
	  uint16_t i;
	  float max,fft_rms;
	  fft_outputbuf[0] = 0;
    for(i=0;i<(FFT_LENGTH/2);i++)
    {
       if(max < fft_outputbuf[i])
       {
          max = fft_outputbuf[i];
          max1 = i;
       }
    }
	  fft_rms = (fft_outputbuf[max1] * 2) / FFT_LENGTH;
	  FFT_UADC_DATA = fft_rms / 1.414;
		//����Ƕ�
		//FFT_PHASE_DATA = atan2(fft_outputbuf[max1*2+1],fft_outputbuf[i*2]);
		//FFT_PHASE_DATA = FFT_PHASE_DATA*180.0f/3.1415926f;
    //����Ƶ��
	  FFT_FREQ_DATA = (Fs / FFT_LENGTH) * max1;
}*/

/**
    * �����������
    * ���ݵ�ѹƵ��ȡ��Ӧ������ֵ��
    * Ƶ��������fft_outputbuf�����У�
    * Ƶ�� = ����Ƶ�� / ��������  *  ������
    * ��ֵ = ����[���] * 2 / ��������
    * ��Чֵ = ��ֵ / 1.414(����2��
		*/
/*void Current_count(void)
{
	  float fft_rms;
	  fft_outputbuf[0] = 0;
	  fft_rms = (fft_outputbuf[max1] * 2) / FFT_LENGTH;
	  FFT_IADC_DATA = fft_rms / 1.414;
}*/

/**
  * �û������趨����
	* �趨�û����ݲ����浽FLASH
	*
	*/
void User_Set_Parameter(void)
{
	 int set_time = 72000000;
	 uint16_t pins_data;
	//ͨ����ʱ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 10;
			 table_array[1] = 1;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[0]% 1000) / 100;
			 table_array[10] = (User_Parameter[0]% 100) / 10;
			 table_array[11] = (User_Parameter[0]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[0]--;
			        if(User_Parameter[0]==0)User_Parameter[0]=500;
						  set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[0]++;
			        if(User_Parameter[0]>500)User_Parameter[0]=1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //��ѹֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 1;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[1]% 1000) / 100;
			 table_array[10] = (User_Parameter[1]% 100) / 10;
			 table_array[11] = (User_Parameter[1]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
			 {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[1]--;
			        if(User_Parameter[1]<230)User_Parameter[1]=300;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[1]++;
			        if(User_Parameter[1]>300)User_Parameter[1]=230;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //��ѹ�ָ�ֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 2;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[2]% 1000) / 100;
			 table_array[10] = (User_Parameter[2]% 100) / 10;
			 table_array[11] = (User_Parameter[2]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[2]--;
			        if(User_Parameter[2]<225)User_Parameter[2]=295;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[2]++;
			        if(User_Parameter[2]>295)User_Parameter[2]=225;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //��ѹ�ָ���ʱ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 3;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[3]% 1000) / 100;
			 table_array[10] = (User_Parameter[3]% 100) / 10;
			 table_array[11] = (User_Parameter[3]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[3]--;
			        if(User_Parameter[3]<1)User_Parameter[3]=500;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[3]++;
			        if(User_Parameter[3]>500)User_Parameter[3]=1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //��ѹ����ֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 4;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[4]% 1000) / 100;
			 table_array[10] = ((User_Parameter[4]% 100) / 10) | 0x20;  //��С����
			 table_array[11] = (User_Parameter[4]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[4]--;
			        if(User_Parameter[4]<1)User_Parameter[4]=300;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[4]++;
			        if(User_Parameter[4]>300)User_Parameter[4]=1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //Ƿѹ�趨ֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 5;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[5]% 1000) / 100;
			 table_array[10] = (User_Parameter[5]% 100) / 10;
			 table_array[11] = (User_Parameter[5]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[5]--;
			        if(User_Parameter[5]<140)User_Parameter[5]=210;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[5]++;
			        if(User_Parameter[5]>210)User_Parameter[5]=140;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //Ƿѹ�ָ�ֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 6;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[6]% 1000) / 100;
			 table_array[10] = (User_Parameter[6]% 100) / 10;
			 table_array[11] = (User_Parameter[6]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[6]--;
			        if(User_Parameter[6]<145)User_Parameter[2]=215;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[6]++;
			        if(User_Parameter[6]>215)User_Parameter[6]=145;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //Ƿѹ�ָ���ʱֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 7;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[7]% 1000) / 100;
			 table_array[10] = (User_Parameter[7]% 100) / 10;
			 table_array[11] = (User_Parameter[7]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[7]--;
			        if(User_Parameter[7]<1)User_Parameter[7]=500;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[7]++;
			        if(User_Parameter[7]>500)User_Parameter[7]=1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //Ƿѹ������ʱֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 8;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[8]% 1000) / 100;
			 table_array[10] = ((User_Parameter[8]% 100) / 10) | 0x20;
			 table_array[11] = (User_Parameter[8]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[8]--;
			        if(User_Parameter[8]<1)User_Parameter[8]=300;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[8]++;
			        if(User_Parameter[8]>300)User_Parameter[8]=1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //�����ѹ���ֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 9;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[9]% 1000) / 100;
			 table_array[10] = ((User_Parameter[9]% 100) / 10) | 0x20;
			 table_array[11] = (User_Parameter[9]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[8]--;
			        if(User_Parameter[8]<1)User_Parameter[8]=300;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[8]++;
			        if(User_Parameter[8]>300)User_Parameter[8]=1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //���಻ƽ��ֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 1;
			 table_array[2] = 0;
			 table_array[9] = (User_Parameter[10]% 1000) / 100;
			 table_array[10] = (User_Parameter[10]% 100) / 10;
			 table_array[11] = (User_Parameter[10]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[10]--;
			        if(User_Parameter[10]<20)User_Parameter[10]=99;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[10]++;
			        if(User_Parameter[10]>99)User_Parameter[10]=20;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //���಻ƽ��ָ�ֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 1;
			 table_array[2] = 1;
			 table_array[9] = (User_Parameter[11]% 1000) / 100;
			 table_array[10] = (User_Parameter[11]% 100) / 10;
			 table_array[11] = (User_Parameter[11]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[11]--;
			        if(User_Parameter[11]<15)User_Parameter[11]=94;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[11]++;
			        if(User_Parameter[11]>94)User_Parameter[11]=15;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //���򱣻���������
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 1;
			 table_array[2] = 2;
		   if(User_Parameter[12])
			 {
					 table_array[9] = 21;
					 table_array[10] = 13;
					 table_array[11] = 15;
			 }
			 else
			 {
					 table_array[9] = 13;
					 table_array[10] = 14;
					 table_array[11] = 14;
			 }
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[12] = 0;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[12] = 1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //��������ֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 12;
			 table_array[1] = 1;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[13]% 1000) / 100;
			 table_array[10] = (User_Parameter[13]% 100) / 10;
			 table_array[11] = (User_Parameter[13]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[13]--;
			        if(User_Parameter[13]<1)User_Parameter[13]=63;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[13]++;
			        if(User_Parameter[13]>63)User_Parameter[13]=1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //�����ָ���ʱֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 12;
			 table_array[1] = 2;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[14]% 1000) / 100;
			 table_array[10] = (User_Parameter[14]% 100) / 10;
			 table_array[11] = (User_Parameter[14]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[14]--;
			        if(User_Parameter[14]<1)User_Parameter[14]=500;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[14]++;
			        if(User_Parameter[14]>500)User_Parameter[14]=1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //����������ʱֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 12;
			 table_array[1] = 3;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[15]% 1000) / 100;
			 table_array[10] = ((User_Parameter[15]% 100) / 10) | 0x20;
			 table_array[11] = (User_Parameter[15]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[15]--;
			        if(User_Parameter[15]<1)User_Parameter[15]=300;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[15]++;
			        if(User_Parameter[15]>300)User_Parameter[15]=1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //��������ֵ����
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 12;
			 table_array[1] = 4;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[16]% 1000) / 100;
			 table_array[10] = (User_Parameter[16]% 100) / 10;
			 table_array[11] = (User_Parameter[16]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[16]--;
			        if(User_Parameter[16]<1)User_Parameter[16]=63;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[16]++;
			        if(User_Parameter[16]>63)User_Parameter[16]=1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 //����������������
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 12;
			 table_array[1] = 5;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[13]% 1000) / 100;
			 table_array[10] = (User_Parameter[13]% 100) / 10;
			 table_array[11] = (User_Parameter[13]% 10) / 1;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Parameter[17]--;
			        if(User_Parameter[17]<1)User_Parameter[17]=20;
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<1000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Parameter[17]++;
			        if(User_Parameter[17]>20)User_Parameter[17]=1;
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 	 //�ָ�Ĭ��ֵ����
	 while(1)
	 {
		   int i;
			 table_array[0] = 16;
			 table_array[1] = 15;
			 table_array[2] = 17;
			 table_array[9] = 0;
			 table_array[10] = 0;
			 table_array[11] = 0;
		   //���¼�����
		   pins_data = gpio_input_data_read(GPIOC);
       if((pins_data & 0x4000)==0)
       {
				   while(i<24000000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
				   if(pins == 0)
					 {
						 	while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S1_Upward_In);
			        User_Set_Def();
						 	set_time = 72000000;
					 }
			 }
			 //���ϼ�����
			 if((pins_data & 0x8000)==0)
       {
				   while(i<24000000)i++;
				   pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
				   if(pins == 0)
					 {
						  while(pins == 0)pins = gpio_input_data_bit_read(GPIOC, S2_Downward_In);
			        User_Set_Def();
						  set_time = 72000000;
					 }
			 }
			 //���ü�����
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
			 {
			     while(i<10000)i++;
				   pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
				   if(pins == 0)
           {
						   while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
					     set_time = 72000000;
						   break;
					 }
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
	 	 //���沢�˳�����
	 while(1)
	 {
		   error_status err_status;
			 table_array[0] = 16;
			 table_array[1] = 15;
			 table_array[2] = 17;
			 table_array[9] = 0;
			 table_array[10] = 0;
			 table_array[11] = 0;
       //�˴��ӱ������ݴ���,�޸�SET_TIME��ʱ��ֵ
		   while(1)
			 {
				  uint16_t i;
		      err_status = flash_write(TEST_FLASH_ADDRESS_START, User_Parameter, TEST_BUFEER_SIZE);
		      if(err_status == SUCCESS){set_time = 24000000;break;}
					while(i<1000)i++;
			 }
			 set_time--;
			 if(set_time == 0)return;
	 }
}

void User_Set_Def(void)
{
	 User_Parameter[0] = 5;
	 User_Parameter[1] = 270;
	 User_Parameter[2] = 265;
	 User_Parameter[3] = 30;
	 User_Parameter[4] = 5;
	 User_Parameter[5] = 170;
	 User_Parameter[6] = 175;
	 User_Parameter[7] = 30;
	 User_Parameter[8] = 5;
	 User_Parameter[9] = 0;
	 User_Parameter[10] = 20;
	 User_Parameter[11] = 20;
	 User_Parameter[12] = 1;
	 User_Parameter[13] = 40;
	 User_Parameter[14] = 30;
	 User_Parameter[15] = 5;
	 User_Parameter[16] = 0;
	 User_Parameter[17] = 1;
}




