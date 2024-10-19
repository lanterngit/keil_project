/**
    *************************************************************
		* @file     adc_sampling.c
    * @brief    main program
		*************************************************************
		*            ˵����
		*
    * ϵͳ�ӳ���
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

extern u8 Fields_Bit;   //����λ�Ĵ���
#define L1DP_BIT  0x01  //L1 DP����λ
#define L2DP_BIT  0x02  //L2 DP����λ
#define L3DP_BIT  0x04  //L3 DP����λ
#define Relay_L1_B        GPIO_PINS_2//PB2
#define Relay_L1_A        GPIO_PINS_10//PB10

extern uint16_t FFT_LENGTH ;  // ��������
extern uint16_t ADC_DMA_data[];  //ADC�������ݼĴ�������
extern uint16_t ADC_Channel_0[];
extern uint16_t ADC_Channel_1[];   //ADC�������ݷ���ͨ��1���顣
extern uint16_t ADC_Channel_2[];   //ADC�������ݷ���ͨ��2���顣
extern uint16_t ADC_Channel_3[];   //ADC�������ݷ���ͨ��3���顣
extern uint16_t ADC_Channel_4[];   //ADC�������ݷ���ͨ��4���顣
extern uint16_t ADC_Channel_5[];   //ADC�������ݷ���ͨ��5���顣
extern uint16_t ADC_Channel_6[];   //ADC�������ݷ���ͨ��6���顣
extern float fft_inputbuf[]; // ����ֵ����
extern float fft_outputbuf[]; //ת��������
extern float FFT_UADC_DATA;  //ת����ĵ�ѹ��Чֵ
extern float FFT_IADC_DATA;  //ת����ĵ�����Чֵ
extern float FFT_FREQ_DATA;  //ת�����Ƶ����Чֵ
extern float FFT_PHASE_DATA;  //ת����ĽǶ�����
extern float UL1_UADC_DATA;  //UL1��ѹ����AN9
extern float UL2_UADC_DATA;  //UL2��ѹ����AN8
extern float UL3_UADC_DATA;  //UL3��ѹ����AN7
extern float IL1_UADC_DATA;   //IL1��ѹ����AN4
extern float IL2_UADC_DATA;   //IL2��������AN5
extern float IL3_UADC_DATA;   //IL3��������AN6
extern float UL1_PADC_DATA;  //UL1�Ƕ�����AN9
extern float UL2_PADC_DATA;  //UL2�Ƕ�����AN8
extern float UL3_PADC_DATA;  //UL3�Ƕ�����AN7
extern float FrequencyCnt;   //Ƶ�����ݼĴ���

extern void FFT_ADC_COMPUTE(uint16_t* data_int);

//������ѹƵ�ʼ������
void data_conversion()
{
		//UL1��ѹ����
	//	right_shift(ADC_Channel_6,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_6);
		//frequency_count();
		UL1_UADC_DATA = FFT_UADC_DATA * 258;
		if(UL1_UADC_DATA > 0)FrequencyCnt = FFT_FREQ_DATA;
		//UL2��ѹ����
		//right_shift(ADC_Channel_5,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_5);
		//frequency_count();
		UL2_UADC_DATA = FFT_UADC_DATA * 258;
		if(UL2_UADC_DATA > 0)FrequencyCnt = FFT_FREQ_DATA;
		//UL3��ѹ����
		//right_shift(ADC_Channel_4,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_4);
		//frequency_count();
		UL3_UADC_DATA = FFT_UADC_DATA * 258;
		if(UL3_UADC_DATA > 0)FrequencyCnt = FFT_FREQ_DATA;
	 //UL3��������
		//right_shift(ADC_Channel_3,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_3);
		//Current_count();
		IL3_UADC_DATA = FFT_IADC_DATA * 58.7f;
		if(IL3_UADC_DATA < 10){IL3_UADC_DATA *= 10;Fields_Bit |= L3DP_BIT;}
		else Fields_Bit &= ~L3DP_BIT;
		//UL2��������
		//right_shift(ADC_Channel_2,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_2);
		//Current_count();
		IL2_UADC_DATA = FFT_IADC_DATA * 58.7f;
		if(IL2_UADC_DATA < 10){IL2_UADC_DATA *= 10;Fields_Bit |= L2DP_BIT;}
		else Fields_Bit &= ~L2DP_BIT;
		//UL1��������
		//right_shift(ADC_Channel_1,FFT_LENGTH);
		FFT_ADC_COMPUTE(ADC_Channel_1);
		//Current_count();
		IL1_UADC_DATA = FFT_IADC_DATA * 58.7f;
		if(IL1_UADC_DATA < 10){IL1_UADC_DATA *= 10;Fields_Bit |= L1DP_BIT;}
		else Fields_Bit &= ~L1DP_BIT;
		
}

/**
  *��Դͨ�Ͽ��Ƴ���
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


