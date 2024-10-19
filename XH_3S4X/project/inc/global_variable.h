/* add user code begin Header */
/**
  **************************************************************************
  * @file     global_variable.h
  * @brief    header file of main interrupt service routines.
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
/* add user code end Header */

/* define to prevent recursive inclusion -------------------------------------*/
#ifndef __GLOBAL_VARIABLE_H
#define __GLOBAL_VARIABLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* includes ------------------------------------------------------------------*/
#include "at32f421.h"
#include "nixietube.h"
#include "at32f421_wk_config.h"
#include "arm_common_tables.h"
#include "adc_sampling.h"
#include "flash.h"	

//#define A Q0
//#define B Q2
//#define C Q4
//#define D Q6
//#define E Q7
//#define F Q1
//#define G Q3
//#define DP Q5
#define LED1  GPIO_PINS_14   //PB14
#define LED2  GPIO_PINS_15   //PB15
#define LED3  GPIO_PINS_8    //PA8
#define LED4  GPIO_PINS_9    //PA9
#define LED5  GPIO_PINS_10   //PA10
#define LED6  GPIO_PINS_11   //PA12
#define LED7  GPIO_PINS_4    //PB4
#define LED8  GPIO_PINS_3    //PB3
#define LED9  GPIO_PINS_15   //PA15
#define LED10 GPIO_PINS_11   //PB10
#define LED11 GPIO_PINS_12   //PB12
#define LED12 GPIO_PINS_13   //PB13
#define LED13 GPIO_PINS_12   //PA12
#define LED14 GPIO_PINS_6    //PF6
#define LED15 GPIO_PINS_7    //PF7
#define LED16 GPIO_PINS_7    //PB7
#define LED17 GPIO_PINS_8    //PB8
#define LED18 GPIO_PINS_9    //PB9
#define DMA1_CHANNEL1_SIZE  6
#define RD1_Power         GPIO_PINS_0//PA0
#define RD2_UnderVoltage  GPIO_PINS_1//PA1
#define RD3_OverVoltage   GPIO_PINS_2//PA2
#define RD4_OverCurrent   GPIO_PINS_3//PA3
#define S1_Upward_In      GPIO_PINS_14//PC14
#define S2_Downward_In    GPIO_PINS_15//PC15
#define S3_Mode_In        GPIO_PINS_0//PF0
#define S4_Confirm_In     GPIO_PINS_1//PF1
#define BUZZER_OUT        GPIO_PINS_13//PC13
#define HC164_DSB         GPIO_PINS_6//PB6
#define HC164_CP          GPIO_PINS_5//PB5
#define Relay_L1_B        GPIO_PINS_2//PB2
#define Relay_L1_A        GPIO_PINS_10//PB10

u8 Fields_Bit;   //����λ�Ĵ���
#define L1DP_BIT  0x01  //L1 DP����λ
#define L2DP_BIT  0x02  //L2 DP����λ
#define L3DP_BIT  0x04  //L3 DP����λ

const uint16_t FFT_LENGTH = 512;  // ��������
const float Fs = 512.0f;    // ����Ƶ��
const uint32_t TEST_FLASH_ADDRESS_START = 0x1FFFF808;
const uint16_t TEST_BUFEER_SIZE = 21;
const char SMG_Duan[10] = {0xD7,0x14,0xCD,0x5D,0x1E,0x5B,0xDB,0x15,0xDF,0x5F};        //���������λ�Ĵ���
const char SMG_Duane[10] = {0x8F,0xD6,0xC2,0xD8,0x8B,0x98,0xCB,0xDC,0x9F,0xC2};       //P,U,C,o,F,n,E,d,A,L

char table_array[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  //�������ʾ��������
char table_data;  //�������ʾ�����Ĵ���

uint16_t User_Parameter[21];  //�û��趨����
//uint16_t Power_Delay;  //�ϵ���ʱ1-500S
//uint16_t U_OverVoltage; //��ѹ����ֵ230-300V
//uint16_t U_OverRestore; //��ѹ�ָ�ֵ225-295V
//uint16_t U_OverResDelay; //��ѹ�ָ���ʱ1-500S
//uint16_t U_OverActDelay; //��ѹ�ָ���ʱ0.1-30SV
//uint16_t U_UnderVoltage; //Ƿѹ����ֵ140-210V
//uint16_t U_UnderRestore; //Ƿѹ�ָ�ֵ145-215V
//uint16_t U_UnderResDelay; //Ƿѹ�ָ���ʱ1-500S
//uint16_t U_UnderActDelay; //Ƿѹ�ָ���ʱ0.1-30SV
//uint16_t U_ErrorVoltage;  //�����ѹ���ֵ-9.5-9.5%
//uint16_t U_BalanceVoltage; //�����ѹ��ƽ��ֵ20-99V
//uint16_t U_BalancerRestro; //�����ѹ��ƽ��ָ�ֵ15-94V
//uint16_t U_PhaseProtect;   //���򱣻�����on /off
//uint16_t I_OverCurrent; //��������ֵ1-63/100
//uint16_t I_OverResDelay; //�����ָ���ʱ1-500S
//uint16_t I_OverActDelay; //�����ָ���ʱ0.1-30SV
//uint16_t I_ErrorCurrent;  //����������ֵ-9.5-9.5%
//uint16_t I_ProtectCunt;  //���������������� 0-20��
//uint16_t U1_PhaseResult;  //U1�Ƕ�ֵ
//uint16_t U2_PhaseResult;  //U2�Ƕ�ֵ
//uint16_t U3_PhaseResult;  //U3�Ƕ�ֵ

uint16_t max1,max2;
uint16_t ADC_ConertedValue;  //ADC��������
uint16_t ValueSampleCnt;  //ADC����������
uint16_t ADC_DMA_data[6];  //ADC�������ݼĴ�������
uint16_t ADC_Channel_1[FFT_LENGTH];   //ADC�������ݷ���ͨ��1���顣
uint16_t ADC_Channel_2[FFT_LENGTH];   //ADC�������ݷ���ͨ��2���顣
uint16_t ADC_Channel_3[FFT_LENGTH];   //ADC�������ݷ���ͨ��3���顣
uint16_t ADC_Channel_4[FFT_LENGTH];   //ADC�������ݷ���ͨ��4���顣
uint16_t ADC_Channel_5[FFT_LENGTH];   //ADC�������ݷ���ͨ��5���顣
uint16_t ADC_Channel_6[FFT_LENGTH];   //ADC�������ݷ���ͨ��6���顣
float fft_inputbuf[FFT_LENGTH*2]; // ����ֵ����
float fft_outputbuf[FFT_LENGTH]; //ת��������
arm_cfft_radix2_instance_f32 scfft;   //��ʼ��FFT����

float FFT_UADC_DATA;  //ת����ĵ�ѹ��Чֵ
float FFT_IADC_DATA;  //ת����ĵ�����Чֵ
float FFT_FREQ_DATA;  //ת�����Ƶ����Чֵ
float FFT_PHASE_DATA;  //ת����ĽǶ�����
float UL1_UADC_DATA;  //UL1��ѹ����AN9
float UL2_UADC_DATA;  //UL2��ѹ����AN8
float UL3_UADC_DATA;  //UL3��ѹ����AN7
float IL1_UADC_DATA;   //IL1��ѹ����AN4
float IL2_UADC_DATA;   //IL2��������AN5
float IL3_UADC_DATA;   //IL3��������AN6
float UL1_PADC_DATA;  //UL1��ѹ����AN9
float UL2_PADC_DATA;  //UL2��ѹ����AN8
float UL3_PADC_DATA;  //UL3��ѹ����AN7
float FrequencyCnt;   //Ƶ�����ݼĴ���
uint16_t ADC_LOOP_DATA;    //����ѭ��������



void FFT_ADC_COMPUTE(uint16_t* data_int);

#ifdef __cplusplus
}
#endif

#endif