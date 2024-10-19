/* add user code begin Header */
/**
  **************************************************************************
  * @file     adc_sampling.h
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
#ifndef __ADC_SAMPLING_H
#define __ADC_SAMPLING_H

//#ifdef __cplusplus
//extern "C" {
//#endif

/* includes ------------------------------------------------------------------*/
#include "at32f421.h"
#include "adc_sampling.h"
	
/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */

/* add user code end private includes */

/* exported types ------------------------------------------------------------*/
/* add user code begin exported types */

/* add user code end exported types */
//struct compx { int real; float imag; };    //定义一个复数结构
/* exported constants --------------------------------------------------------*/
/* add user code begin exported constants */

/* add user code end exported constants */

/* exported macro ------------------------------------------------------------*/
/* add user code begin exported macro */

/* add user code end exported macro */

/* exported functions ------------------------------------------------------- */

void convertIntFloat(uint16_t* data_int, float* fft_inputbuf);
void frequency_count(void);
void User_Set_Parameter(void);
void User_Set_Def(void);
void Current_count(void);
uint16_t* right_shift(uint16_t* right_data,uint16_t right_size);
/* add user code begin exported functions */

/* add user code end exported functions */

//#ifdef __cplusplus
//}
#endif

//#endif
