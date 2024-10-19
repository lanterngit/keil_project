/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
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

/* Includes ------------------------------------------------------------------*/
#include "at32f421_wk_config.h"
#include "at32f421_wk_config.h"
#include "arm_common_tables.h"
#include "adc_sampling.h"
#include "nixietube.h"
#include "flash.h"
#include "subroutine.h"
#include "arm_math.h"
/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */

/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */

/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */
#define DMA1_CHANNEL1_SIZE  7
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
#define POWER_BIT 0x80  //电源状态位

flag_status pins;

const uint16_t FFT_LENGTH = 2048;  // 采样点数
const float Fs = 512.0f;    // 采样频率
const uint32_t TEST_FLASH_ADDRESS_START = 0x1FFFF808;
const uint16_t TEST_BUFEER_SIZE = 21;

char table_array[18]={
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};  //数码管显示数据数组
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

uint16_t ADC_ConertedValue;  //ADC采样数据
uint16_t ValueSampleCnt;  //ADC采样计数器
//uint32_t ADC_DMA_data[7];  //ADC采样数据寄存器数组
uint16_t ADC_Channel_0[FFT_LENGTH];
//uint16_t ADC_Channel_1[FFT_LENGTH];   //ADC采样数据分离通道1数组。
//uint16_t ADC_Channel_2[FFT_LENGTH];   //ADC采样数据分离通道2数组。
//uint16_t ADC_Channel_3[FFT_LENGTH];   //ADC采样数据分离通道3数组。
//uint16_t ADC_Channel_4[FFT_LENGTH];   //ADC采样数据分离通道4数组。
//uint16_t ADC_Channel_5[FFT_LENGTH];   //ADC采样数据分离通道5数组。
//uint16_t ADC_Channel_6[FFT_LENGTH];   //ADC采样数据分离通道6数组。
//float fft_inputbuf[FFT_LENGTH*2]; // 采样值数组
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

/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */
void FFT_ADC_COMPUTE(uint16_t* data_int)
{
	  uint16_t i;
	  // 将int型数组转化为浮点型数组，表示采样值
		//convertIntFloat(data_int,fft_inputbuf);
		FFT_UADC_DATA = 0;
	  for(i=0;i<FFT_LENGTH;i++)
	  {
			 fft_outputbuf[i] = (1.2f / ADC_Channel_0[i]) * (data_int[i]-0x0793);//ADC实部
			 //fft_inputbuf[2*i] = (1.2f / ADC_Channel_0[i]) * data_int[i];//ADC实部
			 //fft_inputbuf[2*i+1] = 0.0f;//虚部为0
			 FFT_UADC_DATA += fft_outputbuf[i] * fft_outputbuf[i];
		}
		FFT_UADC_DATA = sqrt(FFT_UADC_DATA / FFT_LENGTH);
		//FFT计算（基2）
    //arm_cfft_radix2_f32(&scfft,fft_inputbuf);
		//计算模值
		//arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);
}
//此处加入相位角计算程序
/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  wk_system_clock_config();
  wk_periph_clock_config();
  wk_nvic_config();
  wk_dma1_channel1_init();
  //wk_dma_channel_config(DMA1_CHANNEL1, (uint32_t)&ADC1->odt, (uint32_t)ADC_DMA_data, DMA1_CHANNEL1_SIZE);
	wk_dma_channel_config(DMA1_CHANNEL1, (uint32_t)&ADC1->odt, (uint32_t)ADC_Channel_0, DMA1_CHANNEL1_SIZE);
	dma_flag_clear(DMA1_FDT1_FLAG);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);	
  wk_gpio_config();
	gpio_bits_write(GPIOA,GPIO_PINS_0 | GPIO_PINS_1 | GPIO_PINS_2 | GPIO_PINS_3,TRUE);
	wk_adc1_init();
  wk_tmr3_init();
  wk_tmr6_init();
	wk_tmr14_init();
	tmr_flag_clear(TMR3,TMR_OVF_FLAG);
	tmr_interrupt_enable(TMR6,TMR_OVF_INT,TRUE);  //TMR6中断允许
	tmr_interrupt_enable(TMR3,TMR_OVF_INT,TRUE);  //TMR3中断允许
	dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);
	//tmr_interrupt_enable(TMR14,TMR_OVF_INT,TRUE);  //TMR4中断允许
	//flash_read(TEST_FLASH_ADDRESS_START, User_Parameter, TEST_BUFEER_SIZE);
	tmr_counter_enable(TMR3, TRUE); //开启TMR3
	start_countdown(5); //开机倒计时程序
	//power_action(1);  //开启电源
	arm_cfft_radix2_init_f32(&scfft,FFT_LENGTH,0,1);//初始化scfft结构体，设定FFT相关参数，
  //tmr_counter_enable(TMR3, TRUE); //开启TMR3
  while(1)
  {
  	 if(ADC_LOOP_DATA == FFT_LENGTH)
		 {
			 // tmr_counter_enable(TMR3, FALSE); //开启TMR3
			  data_conversion();  //电流电压频率计算程序
			  fft_to_array();
			 // tmr_flag_clear(TMR3,TMR_OVF_FLAG);
			 // tmr_counter_enable(TMR3, TRUE); //开启TMR3
				ADC_LOOP_DATA = 0;
		 }
		 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
		 if(pins == 0)
		 {
			 int i;
			 while(i<10000)i++;
			 pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			 if(pins == 0)
       {
				  while(pins == 0)pins = gpio_input_data_bit_read(GPIOF, S3_Mode_In);
			    User_Set_Parameter();
			 }
		 }
		 //此处加入电压，电流比较程序
		 pins = gpio_input_data_bit_read(GPIOF, S4_Confirm_In);
		 if(pins ==0)
		 {
			 if(Fields_Bit & POWER_BIT)
			 {
			     power_action(0);
				   Fields_Bit &= ~POWER_BIT;
			 }
			 else
			 {
			     power_action(1);
				   Fields_Bit |= POWER_BIT;
			 }
		 }
  }
}
