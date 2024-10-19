/**
    *************************************************************
		* @file     adc_sampling.c
    * @brief    main program
		*************************************************************
		*            说明：
		*
    * 系统子程序
    * 
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
#include "subroutine.h"

extern u8 Fields_Bit;   //变量位寄存器
#define L1DP_BIT  0x01  //L1 DP允许位
#define L2DP_BIT  0x02  //L2 DP允许位
#define L3DP_BIT  0x04  //L3 DP允许位
#define Relay_L1_B        GPIO_PINS_2//PB2
#define Relay_L1_A        GPIO_PINS_10//PB10

extern uint16_t FFT_LENGTH ;  // 采样点数
extern uint16_t ADC_DMA_data[];  //ADC采样数据寄存器数组
extern uint16_t ADC_Channel_0[];
extern uint16_t ADC_Channel_1[];   //ADC采样数据分离通道1数组。
extern uint16_t ADC_Channel_2[];   //ADC采样数据分离通道2数组。
extern uint16_t ADC_Channel_3[];   //ADC采样数据分离通道3数组。
extern uint16_t ADC_Channel_4[];   //ADC采样数据分离通道4数组。
extern uint16_t ADC_Channel_5[];   //ADC采样数据分离通道5数组。
extern uint16_t ADC_Channel_6[];   //ADC采样数据分离通道6数组。
extern float fft_inputbuf[]; // 采样值数组
extern float fft_outputbuf[]; //转换后数据
extern float FFT_UADC_DATA;  //转换后的电压有效值
extern float FFT_IADC_DATA;  //转换后的电流有效值
extern float FFT_FREQ_DATA;  //转换后的频率有效值
extern float FFT_PHASE_DATA;  //转换后的角度数据
extern float UL1_UADC_DATA;  //UL1电压数据AN9
extern float UL2_UADC_DATA;  //UL2电压数据AN8
extern float UL3_UADC_DATA;  //UL3电压数据AN7
extern float IL1_UADC_DATA;   //IL1电压数据AN4
extern float IL2_UADC_DATA;   //IL2电流数据AN5
extern float IL3_UADC_DATA;   //IL3电流数据AN6
extern float UL1_PADC_DATA;  //UL1角度数据AN9
extern float UL2_PADC_DATA;  //UL2角度数据AN8
extern float UL3_PADC_DATA;  //UL3角度数据AN7
extern float FrequencyCnt;   //频率数据寄存器

extern void FFT_ADC_COMPUTE(uint16_t* data_int);

//电流电压频率计算程序
void data_conversion()
{
		//UL1电压计算
	//	right_shift(ADC_Channel_6,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_6);
		//frequency_count();
		UL1_UADC_DATA = FFT_UADC_DATA * 258;
		if(UL1_UADC_DATA > 0)FrequencyCnt = FFT_FREQ_DATA;
		//UL2电压计算
		//right_shift(ADC_Channel_5,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_5);
		//frequency_count();
		UL2_UADC_DATA = FFT_UADC_DATA * 258;
		if(UL2_UADC_DATA > 0)FrequencyCnt = FFT_FREQ_DATA;
		//UL3电压计算
		//right_shift(ADC_Channel_4,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_4);
		//frequency_count();
		UL3_UADC_DATA = FFT_UADC_DATA * 258;
		if(UL3_UADC_DATA > 0)FrequencyCnt = FFT_FREQ_DATA;
	 //UL3电流计算
		//right_shift(ADC_Channel_3,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_3);
		//Current_count();
		IL3_UADC_DATA = FFT_IADC_DATA * 58.7f;
		if(IL3_UADC_DATA < 10){IL3_UADC_DATA *= 10;Fields_Bit |= L3DP_BIT;}
		else Fields_Bit &= ~L3DP_BIT;
		//UL2电流计算
		//right_shift(ADC_Channel_2,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_2);
		//Current_count();
		IL2_UADC_DATA = FFT_IADC_DATA * 58.7f;
		if(IL2_UADC_DATA < 10){IL2_UADC_DATA *= 10;Fields_Bit |= L2DP_BIT;}
		else Fields_Bit &= ~L2DP_BIT;
		//UL1电流计算
		//right_shift(ADC_Channel_1,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_1);
		//Current_count();
		IL1_UADC_DATA = FFT_IADC_DATA * 58.7f;
		if(IL1_UADC_DATA < 10){IL1_UADC_DATA *= 10;Fields_Bit |= L1DP_BIT;}
		else Fields_Bit &= ~L1DP_BIT;
		
}

/**
  *电源通断控制程序
  */
void power_action(char state)
{
	 uint16_t i;
	 if(state == 0)
	 {
		  gpio_bits_write(GPIOB,Relay_L1_A,TRUE);
		  gpio_bits_write(GPIOB,Relay_L1_B,FALSE);
		  while(i<3000)i++;
		  gpio_bits_write(GPIOB,Relay_L1_A,FALSE);
	 }
	 else
		 	 {
		  gpio_bits_write(GPIOB,Relay_L1_A,FALSE);
		  gpio_bits_write(GPIOB,Relay_L1_B,TRUE);
		  while(i<3000)i++;
		  gpio_bits_write(GPIOB,Relay_L1_B,FALSE);
	 }
}


