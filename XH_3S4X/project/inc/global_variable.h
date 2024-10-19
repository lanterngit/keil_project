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

u8 Fields_Bit;   //变量位寄存器
#define L1DP_BIT  0x01  //L1 DP允许位
#define L2DP_BIT  0x02  //L2 DP允许位
#define L3DP_BIT  0x04  //L3 DP允许位

const uint16_t FFT_LENGTH = 512;  // 采样点数
const float Fs = 512.0f;    // 采样频率
const uint32_t TEST_FLASH_ADDRESS_START = 0x1FFFF808;
const uint16_t TEST_BUFEER_SIZE = 21;
const char SMG_Duan[10] = {0xD7,0x14,0xCD,0x5D,0x1E,0x5B,0xDB,0x15,0xDF,0x5F};        //数码管数据位寄存器
const char SMG_Duane[10] = {0x8F,0xD6,0xC2,0xD8,0x8B,0x98,0xCB,0xDC,0x9F,0xC2};       //P,U,C,o,F,n,E,d,A,L

char table_array[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  //数码管显示数据数组
char table_data;  //数码管显示计数寄存器

uint16_t User_Parameter[21];  //用户设定参数
//uint16_t Power_Delay;  //上电延时1-500S
//uint16_t U_OverVoltage; //过压保护值230-300V
//uint16_t U_OverRestore; //过压恢复值225-295V
//uint16_t U_OverResDelay; //过压恢复延时1-500S
//uint16_t U_OverActDelay; //过压恢复延时0.1-30SV
//uint16_t U_UnderVoltage; //欠压保护值140-210V
//uint16_t U_UnderRestore; //欠压恢复值145-215V
//uint16_t U_UnderResDelay; //欠压恢复延时1-500S
//uint16_t U_UnderActDelay; //欠压恢复延时0.1-30SV
//uint16_t U_ErrorVoltage;  //三相电压误差值-9.5-9.5%
//uint16_t U_BalanceVoltage; //三相电压不平衡值20-99V
//uint16_t U_BalancerRestro; //三相电压不平衡恢复值15-94V
//uint16_t U_PhaseProtect;   //相序保护开关on /off
//uint16_t I_OverCurrent; //过流保护值1-63/100
//uint16_t I_OverResDelay; //过流恢复延时1-500S
//uint16_t I_OverActDelay; //过流恢复延时0.1-30SV
//uint16_t I_ErrorCurrent;  //三相电流误差值-9.5-9.5%
//uint16_t I_ProtectCunt;  //连续过流保护次数 0-20次
//uint16_t U1_PhaseResult;  //U1角度值
//uint16_t U2_PhaseResult;  //U2角度值
//uint16_t U3_PhaseResult;  //U3角度值

uint16_t max1,max2;
uint16_t ADC_ConertedValue;  //ADC采样数据
uint16_t ValueSampleCnt;  //ADC采样计数器
uint16_t ADC_DMA_data[6];  //ADC采样数据寄存器数组
uint16_t ADC_Channel_1[FFT_LENGTH];   //ADC采样数据分离通道1数组。
uint16_t ADC_Channel_2[FFT_LENGTH];   //ADC采样数据分离通道2数组。
uint16_t ADC_Channel_3[FFT_LENGTH];   //ADC采样数据分离通道3数组。
uint16_t ADC_Channel_4[FFT_LENGTH];   //ADC采样数据分离通道4数组。
uint16_t ADC_Channel_5[FFT_LENGTH];   //ADC采样数据分离通道5数组。
uint16_t ADC_Channel_6[FFT_LENGTH];   //ADC采样数据分离通道6数组。
float fft_inputbuf[FFT_LENGTH*2]; // 采样值数组
float fft_outputbuf[FFT_LENGTH]; //转换后数据
arm_cfft_radix2_instance_f32 scfft;   //初始化FFT参数

float FFT_UADC_DATA;  //转换后的电压有效值
float FFT_IADC_DATA;  //转换后的电流有效值
float FFT_FREQ_DATA;  //转换后的频率有效值
float FFT_PHASE_DATA;  //转换后的角度数据
float UL1_UADC_DATA;  //UL1电压数据AN9
float UL2_UADC_DATA;  //UL2电压数据AN8
float UL3_UADC_DATA;  //UL3电压数据AN7
float IL1_UADC_DATA;   //IL1电压数据AN4
float IL2_UADC_DATA;   //IL2电流数据AN5
float IL3_UADC_DATA;   //IL3电流数据AN6
float UL1_PADC_DATA;  //UL1电压数据AN9
float UL2_PADC_DATA;  //UL2电压数据AN8
float UL3_PADC_DATA;  //UL3电压数据AN7
float FrequencyCnt;   //频率数据寄存器
uint16_t ADC_LOOP_DATA;    //采样循环计数器



void FFT_ADC_COMPUTE(uint16_t* data_int);

#ifdef __cplusplus
}
#endif

#endif