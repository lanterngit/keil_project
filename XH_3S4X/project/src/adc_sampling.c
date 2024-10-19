/**
    *************************************************************
		* @file     adc_sampling.c
    * @brief    main program
		*************************************************************
		*            说明：
		*
    * 电压电流采样程序
    * 对电压和电流进行采样，并将采样数据进行整理。
		* TMR3为采样定时器，采样频率为2000HZ
		* ADC时钟为4MHZ  采样时间7.5个ADCCLK,
		* 采样方式7路ADC轮流采样,每组采样78个数据
		* 通过DMA将数据发送到显示数组中。
		* 判断电压和电流是否合格,
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

const char SMG_Duan[21] = {0xD7,0x14,0xCD,0x5D,0x1E,0x5B,0xDB,0x15,0xDF,0x5F,0x8F,0xD6,0xC2,0xD8,0x8B,0x98,0xCB,0xDC,0x9F,0xC2,0x00};        //数码管数据位寄存器
//const char SMG_Duane[10] = {0x8F,0xD6,0xC2,0xD8,0x8B,0x98,0xCB,0xDC,0x9F,0xC2};       //P,U,C,o,F,n,E,d,A,L
extern char table_array[];  //显示数据数组
extern u16 data_int[];   //采样数据临时寄存器数组，用于暂存采样数据。
extern const uint16_t FFT_LENGTH;  // 采样点数
extern const float Fs;    // 采样频率
extern uint16_t User_Parameter[18];  //用户设定参数
extern const uint32_t TEST_FLASH_ADDRESS_START;
extern const uint16_t TEST_BUFEER_SIZE;

//extern uint16_t ADC_DMA_data[];  //ADC采样数据寄存器数组
extern uint16_t ADC_Channel_0[];   //ADC采样数据分离通道1数组。
//extern uint16_t ADC_Channel_2[];   //ADC采样数据分离通道2数组。
//extern uint16_t ADC_Channel_3[];   //ADC采样数据分离通道3数组。
extern float fft_outputbuf[]; //转换后数据

extern float FFT_UADC_DATA;//ADC采样有效值数据
extern float FFT_IADC_DATA;  //转换后的电流有效值
extern float FFT_FREQ_DATA;  //转换后的频率值
extern float FFT_PHASE_DATA;  //转换后的角度数据
	
extern flag_status pins;

uint16_t max1,max2;


/**
  *数组数据右移4位程序
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
    * 整数OR浮点数转换程序
    * 将int型数组转化为浮点型数组，表示采样值
		* 采样数据分为实部和虚部
		* 实部为采样的实际数据
		* 虚部为0
    * 采样数据保存在data_int数组内。
    * 转换后数据保存在fft_inputbuf数组内.  
		*/
/*void convertIntFloat(uint16_t* data_int, float* fft_inputbuf)
{
	  uint16_t i;
    for (i = 0; i < FFT_LENGTH; i++) 
	  {
        //fft_inputbuf[2*i] = (short)data_int[i]*3.3f/4095.0f;//ADC实部
			  fft_inputbuf[2*i] = (1.2f / ADC_Channel_0[i]) * data_int[i];//ADC实部
			  fft_inputbuf[2*i+1] = 0.0f;//虚部为0
    }
}*/


/**
    * 电压，频率计算程序
    * 读取频谱数据，找到数组0以外的最大数据，找出对应的频率和幅度。
    * 频谱数据在fft_outputbuf数组中，
    * 数组0幅度最大，为直流分量，不做处理。
    * 除数组0外的数组为相应频率的谐波分量。幅度最大的数为所要采样的数据
    * 数组0-FFT_LENGTH/2为正频率，数组FFT_LENGTH/2-FFT_LENGTH为负频率
    * 检索最大数得到相应数组标号，
    * 频率 = 采样频率 / 采样次数  *  数组标号
    * 幅值 = 数组[标号] * 2 / 采样次数
    * 有效值 = 幅值 / 1.414(根号2）
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
		//计算角度
		//FFT_PHASE_DATA = atan2(fft_outputbuf[max1*2+1],fft_outputbuf[i*2]);
		//FFT_PHASE_DATA = FFT_PHASE_DATA*180.0f/3.1415926f;
    //计算频率
	  FFT_FREQ_DATA = (Fs / FFT_LENGTH) * max1;
}*/

/**
    * 电流计算程序
    * 根据电压频率取相应电流幅值。
    * 频谱数据在fft_outputbuf数组中，
    * 频率 = 采样频率 / 采样次数  *  数组标号
    * 幅值 = 数组[标号] * 2 / 采样次数
    * 有效值 = 幅值 / 1.414(根号2）
		*/
/*void Current_count(void)
{
	  float fft_rms;
	  fft_outputbuf[0] = 0;
	  fft_rms = (fft_outputbuf[max1] * 2) / FFT_LENGTH;
	  FFT_IADC_DATA = fft_rms / 1.414;
}*/

/**
  * 用户数据设定程序
	* 设定用户数据并保存到FLASH
	*
	*/
void User_Set_Parameter(void)
{
	 int set_time = 72000000;
	 uint16_t pins_data;
	//通电延时设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 10;
			 table_array[1] = 1;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[0]% 1000) / 100;
			 table_array[10] = (User_Parameter[0]% 100) / 10;
			 table_array[11] = (User_Parameter[0]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //过压值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 1;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[1]% 1000) / 100;
			 table_array[10] = (User_Parameter[1]% 100) / 10;
			 table_array[11] = (User_Parameter[1]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //过压恢复值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 2;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[2]% 1000) / 100;
			 table_array[10] = (User_Parameter[2]% 100) / 10;
			 table_array[11] = (User_Parameter[2]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //过压恢复延时设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 3;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[3]% 1000) / 100;
			 table_array[10] = (User_Parameter[3]% 100) / 10;
			 table_array[11] = (User_Parameter[3]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //过压动作值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 4;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[4]% 1000) / 100;
			 table_array[10] = ((User_Parameter[4]% 100) / 10) | 0x20;  //加小数点
			 table_array[11] = (User_Parameter[4]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //欠压设定值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 5;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[5]% 1000) / 100;
			 table_array[10] = (User_Parameter[5]% 100) / 10;
			 table_array[11] = (User_Parameter[5]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //欠压恢复值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 6;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[6]% 1000) / 100;
			 table_array[10] = (User_Parameter[6]% 100) / 10;
			 table_array[11] = (User_Parameter[6]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //欠压恢复延时值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 7;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[7]% 1000) / 100;
			 table_array[10] = (User_Parameter[7]% 100) / 10;
			 table_array[11] = (User_Parameter[7]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //欠压动作延时值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 8;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[8]% 1000) / 100;
			 table_array[10] = ((User_Parameter[8]% 100) / 10) | 0x20;
			 table_array[11] = (User_Parameter[8]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //三相电压误差值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 9;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[9]% 1000) / 100;
			 table_array[10] = ((User_Parameter[9]% 100) / 10) | 0x20;
			 table_array[11] = (User_Parameter[9]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //三相不平衡值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 1;
			 table_array[2] = 0;
			 table_array[9] = (User_Parameter[10]% 1000) / 100;
			 table_array[10] = (User_Parameter[10]% 100) / 10;
			 table_array[11] = (User_Parameter[10]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //三相不平衡恢复值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 11;
			 table_array[1] = 1;
			 table_array[2] = 1;
			 table_array[9] = (User_Parameter[11]% 1000) / 100;
			 table_array[10] = (User_Parameter[11]% 100) / 10;
			 table_array[11] = (User_Parameter[11]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //相序保护开关设置
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
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //过流保护值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 12;
			 table_array[1] = 1;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[13]% 1000) / 100;
			 table_array[10] = (User_Parameter[13]% 100) / 10;
			 table_array[11] = (User_Parameter[13]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //过流恢复延时值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 12;
			 table_array[1] = 2;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[14]% 1000) / 100;
			 table_array[10] = (User_Parameter[14]% 100) / 10;
			 table_array[11] = (User_Parameter[14]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //过流动作延时值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 12;
			 table_array[1] = 3;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[15]% 1000) / 100;
			 table_array[10] = ((User_Parameter[15]% 100) / 10) | 0x20;
			 table_array[11] = (User_Parameter[15]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //过流保护值设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 12;
			 table_array[1] = 4;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[16]% 1000) / 100;
			 table_array[10] = (User_Parameter[16]% 100) / 10;
			 table_array[11] = (User_Parameter[16]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 //过流保护次数设置
	 while(1)
	 {
		   uint16_t i;
			 table_array[0] = 12;
			 table_array[1] = 5;
			 table_array[2] = 21;
			 table_array[9] = (User_Parameter[13]% 1000) / 100;
			 table_array[10] = (User_Parameter[13]% 100) / 10;
			 table_array[11] = (User_Parameter[13]% 10) / 1;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 	 //恢复默认值设置
	 while(1)
	 {
		   int i;
			 table_array[0] = 16;
			 table_array[1] = 15;
			 table_array[2] = 17;
			 table_array[9] = 0;
			 table_array[10] = 0;
			 table_array[11] = 0;
		   //向下键处理
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
			 //向上键处理
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
			 //设置键处理
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
	 	 //保存并退出设置
	 while(1)
	 {
		   error_status err_status;
			 table_array[0] = 16;
			 table_array[1] = 15;
			 table_array[2] = 17;
			 table_array[9] = 0;
			 table_array[10] = 0;
			 table_array[11] = 0;
       //此处加保存数据代码,修改SET_TIME延时数值
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




