/**
    *************************************************************
		* @file     nixierube.c
    * @brief    main program
		*************************************************************
		*            说明：
		*
    * 数码管字符生成显示程序
    * 生成数码管显示字符，将数组数据转换为显示码。
		* 刷新数码管显示状态。将显示码循环显示。
		* 数码管刷新时间为1ms,
		*/
/* add user code end Header */
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
#define LED6  GPIO_PINS_11   //PA11
#define LED7  GPIO_PINS_4    //PB4
#define LED8  GPIO_PINS_3    //PB3
#define LED9  GPIO_PINS_15   //PA15
#define LED10 GPIO_PINS_11   //PB11
#define LED11 GPIO_PINS_12   //PB12
#define LED12 GPIO_PINS_13   //PB13
#define LED13 GPIO_PINS_12   //PA12
#define LED14 GPIO_PINS_6    //PF6
#define LED15 GPIO_PINS_7    //PF7
#define LED16 GPIO_PINS_7    //PB7
#define LED17 GPIO_PINS_8    //PB8
#define LED18 GPIO_PINS_9    //PB9
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
/* Includes ------------------------------------------------------------------*/
#include "at32f421_wk_config.h"
#include "at32f421_conf.h"
#include "nixietube.h"
#include "at32f421_tmr.h"
#include "at32f421_int.h"


extern u8 Fields_Bit;   //变量位寄存器
#define L1DP_BIT  0x01  //L1 DP允许位
#define L2DP_BIT  0x02  //L2 DP允许位
#define L3DP_BIT  0x04  //L3 DP允许位
//#define A Q0
//#define B Q2
//#define C Q4
//#define D Q6
//#define E Q7
//#define F Q1
//#define G Q3
//#define DP Q5
extern char table_array[];  //显示数据数组
extern char table_data;  //数码管显示计数寄存器
extern const char SMG_Duan[];
//const char SMG_Duan[10] = {0xD7,0x14,0xCD,0x5D,0x1E,0x5B,0xDB,0x15,0xDF,0x5F};        //数码管数据位寄存器
//extern const char SMG_Duane[];       //P,U,C,o,F,n,E,d,A,L

extern float UL1_UADC_DATA;  //UL1电压数据
extern float UL2_UADC_DATA;  //UL2电压数据
extern float UL3_UADC_DATA;  //UL3电压数据
extern float IL1_UADC_DATA;   //IL1电压数据AN5
extern float IL2_UADC_DATA;   //IL2电流数据AN9
extern float IL3_UADC_DATA;   //IL3电流数据AN9
extern float FrequencyCnt;   //频率数据寄存器
/**
    * 显示数组数据刷新程序
		* 将FFT处理好的数据转换到显示数组内
		*/
void fft_to_array(void)
{
	  table_array[0] = ((int)UL1_UADC_DATA % 1000) / 100;
		table_array[1] = ((int)UL1_UADC_DATA % 100) / 10;
		table_array[2] = ((int)UL1_UADC_DATA % 10) / 1;
	  table_array[3] = ((int)UL2_UADC_DATA % 1000) / 100;
		table_array[4] = ((int)UL2_UADC_DATA % 100) / 10;
    table_array[5] = ((int)UL2_UADC_DATA % 10) / 1;
	  table_array[6] = ((int)UL3_UADC_DATA % 1000) / 100;
		table_array[7] = ((int)UL3_UADC_DATA % 100) / 10;
	  table_array[8] = ((int)UL3_UADC_DATA % 10) / 1;
	  table_array[9] = ((int)IL1_UADC_DATA % 1000) / 100;
		table_array[10] = ((int)IL1_UADC_DATA % 100) / 10;
		table_array[11] = ((int)IL1_UADC_DATA % 10) / 1;
	  table_array[12] = ((int)IL2_UADC_DATA % 1000) / 100;
		table_array[13] = ((int)IL2_UADC_DATA % 100) / 10;
		table_array[14] = ((int)IL2_UADC_DATA % 10) / 1;
		table_array[15] = ((int)FrequencyCnt % 1000) / 100;
		table_array[16] = ((int)FrequencyCnt % 100) / 10;
		table_array[17] = ((int)FrequencyCnt % 10) / 1;
}

/**
    * 数码管显示刷新程序
    * 刷新数码管显示状态。将显示码循环显示。
		* TMR6为刷新定时器
		* 数码管刷新频率1000HZ,
    * 显示数据保存在table_data[]数组内。
		*/
void table_bits_refresh(void)
{
	  SMG_display_HC164(table_array[table_data]);
	  SMG_display_BIT(table_data);
}

/**
    * 将数码管显示位发送到输出脚
    * GPIOA LED3,LED4,LED5,LED6,LED9,LED13
		* GPIOB LED1,LED2,LED7,LED8,LED10,LED11,LED12,LED16,LED17,LED18
    * GPIOF LED14,LED15
		*/
void SMG_display_BIT(char codereg)
{
	  switch(codereg)
		{
			case 0:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOB,LED1,FALSE);
				break;
			}
			case 1:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOB,LED2,FALSE);
				break;
			}
			case 2:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOA,LED3,FALSE);
				break;
			}
			case 3:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOA,LED4,FALSE);
				break;
			}
			case 4:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOA,LED5,FALSE);
				break;
			}
			case 5:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOA,LED6,FALSE);
				break;
			}
			case 6:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOB,LED7,FALSE);
				break;
			}
			case 7:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOB,LED8,FALSE);
				break;
			}
			case 8:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOA,LED9,FALSE);
				break;
			}
			case 9:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOB,LED10,FALSE);
				break;
			}
			case 10:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOB,LED11,FALSE);
				break;
			}
			case 11:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOB,LED12,FALSE);
				break;
			}
			case 12:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOA,LED13,FALSE);
				break;
			}
			case 13:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOF,LED14,FALSE);
				break;
			}
			case 14:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOF,LED15,FALSE);
				break;
			}
			case 15:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOB,LED16,FALSE);
				break;
			}
			case 16:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOB,LED17,FALSE);
				break;
			}
			case 17:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				gpio_bits_write(GPIOB,LED18,FALSE);
				break;
			}
			default:
			{
				gpio_bits_write(GPIOA,LED3 | LED4 | LED5 | LED6 | LED9 | LED13,TRUE);
				gpio_bits_write(GPIOB,LED1 | LED2 | LED7 | LED8 | LED10 | LED11 | LED12 | LED16 | LED17 | LED18,TRUE);
				gpio_bits_write(GPIOF,LED14 | LED15,TRUE);
				break;
			}
		}
}


/**
    * 将数码管显示数据发送到HC164内
    * 对应脚位E,Q7 D,Q6 DP,Q5 C,Q4 G,Q3 B,Q2 F,Q1 A,Q0
    * DP_BIT为小数点显示允许位
		*/
void SMG_display_HC164(char datareg)
{
	    char SMG_buffer,i;
	    SMG_buffer = SMG_Duan[datareg];
	    if(table_data == 10){if(Fields_Bit & L1DP_BIT)SMG_buffer |= 0x20;}
			if(table_data == 13){if(Fields_Bit & L2DP_BIT)SMG_buffer |= 0x20;}
			if(table_data == 16){if(Fields_Bit & L3DP_BIT)SMG_buffer |= 0x20;}
			for(i=0;i<8;i++)
			{
				  if(SMG_buffer & 0x80){gpio_bits_write(GPIOB,HC164_DSB,TRUE);}
					else{gpio_bits_write(GPIOB,HC164_DSB,FALSE);}
					gpio_bits_write(GPIOB,HC164_CP,FALSE);
					gpio_bits_write(GPIOB,HC164_CP,TRUE);
					SMG_buffer <<= 1;
			}
}

/**
  *开机到计时程序
  */
void start_countdown(int16_t delay)
{
	tmr_base_init(TMR14, 6000, 4000);
	tmr_flag_clear(TMR14,TMR_OVF_FLAG);
	tmr_counter_enable(TMR14, TRUE);
	table_array[2] = 21;
	table_array[3] = 21;
	table_array[4] = 21;
	table_array[5] = 21;
	table_array[6] = 21;
	table_array[7] = 21;
	table_array[8] = 21;
	table_array[12] = 21;
	table_array[13] = 21;
	table_array[14] = 21;
	table_array[15] = 21;
	table_array[16] = 21;
	table_array[17] = 21;
	while(1)
	{
			table_array[0] = 10;
			table_array[1] = 1;
			table_array[9] = (delay %1000)/100;
			table_array[10] = (delay %100)/10;
			table_array[11] = (delay %10)/1;
			while(1)
			{
					if(tmr_flag_get(TMR14,TMR_OVF_FLAG) != RESET)
					{
							tmr_flag_clear(TMR14,TMR_OVF_FLAG);
						  break;
					}
			}
			delay--;
			if(delay < 0)
			{
				 tmr_counter_enable(TMR14, FALSE);
				 return;
			}
	}
}
/**
  * 延时程序
	* 延时时间为100MS
	*/
void Delay(uint16_t delay_data)
{
   uint16_t i;
	 tmr_counter_enable(TMR14, TRUE);
   for(i=0;i<delay_data;i++)
	 {
	     while(1)
			 {
				  if(tmr_flag_get(TMR14,TMR_OVF_FLAG) != RESET)
					{
						 tmr_flag_clear(TMR14,TMR_OVF_FLAG);
					}
			 }
   }
	 tmr_counter_enable(TMR14, FALSE);
}

