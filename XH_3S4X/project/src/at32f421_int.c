/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f421_int.c
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32f421_int.h"
#include "nixietube.h"
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
extern char table_array[];  //???????????
extern char table_data;  //??????????????????
extern const uint16_t FFT_LENGTH;  // ��������
extern uint16_t ADC_DMA_data[];  //ADC�������ݼĴ�������
extern uint16_t ADC_LOOP_DATA;    //����ѭ��������
extern uint16_t ADC_Channel_0[];
extern uint16_t ADC_Channel_1[];   //ADC�������ݷ���ͨ��1���顣
extern uint16_t ADC_Channel_2[];   //ADC�������ݷ���ͨ��2���顣
extern uint16_t ADC_Channel_3[];   //ADC�������ݷ���ͨ��3���顣
extern uint16_t ADC_Channel_4[];   //ADC�������ݷ���ͨ��4���顣
extern uint16_t ADC_Channel_5[];   //ADC�������ݷ���ͨ��5���顣
extern uint16_t ADC_Channel_6[];   //ADC�������ݷ���ͨ��6���顣
/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/* external variables ---------------------------------------------------------*/
/* add user code begin external variables */

/* add user code end external variables */

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
  /* add user code begin NonMaskableInt_IRQ 0 */

  /* add user code end NonMaskableInt_IRQ 0 */

  /* add user code begin NonMaskableInt_IRQ 1 */

  /* add user code end NonMaskableInt_IRQ 1 */
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
void HardFault_Handler(void)
{
  /* add user code begin HardFault_IRQ 0 */

  /* add user code end HardFault_IRQ 0 */
  /* go to infinite loop when hard fault exception occurs */
  while (1)
  {
    /* add user code begin W1_HardFault_IRQ 0 */

    /* add user code end W1_HardFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* add user code begin MemoryManagement_IRQ 0 */

  /* add user code end MemoryManagement_IRQ 0 */
  /* go to infinite loop when memory manage exception occurs */
  while (1)
  {
    /* add user code begin W1_MemoryManagement_IRQ 0 */

    /* add user code end W1_MemoryManagement_IRQ 0 */
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* add user code begin BusFault_IRQ 0 */

  /* add user code end BusFault_IRQ 0 */
  /* go to infinite loop when bus fault exception occurs */
  while (1)
  {
    /* add user code begin W1_BusFault_IRQ 0 */

    /* add user code end W1_BusFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* add user code begin UsageFault_IRQ 0 */

  /* add user code end UsageFault_IRQ 0 */
  /* go to infinite loop when usage fault exception occurs */
  while (1)
  {
    /* add user code begin W1_UsageFault_IRQ 0 */

    /* add user code end W1_UsageFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
  /* add user code begin SVCall_IRQ 0 */

  /* add user code end SVCall_IRQ 0 */
  /* add user code begin SVCall_IRQ 1 */

  /* add user code end SVCall_IRQ 1 */
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
  /* add user code begin DebugMonitor_IRQ 0 */

  /* add user code end DebugMonitor_IRQ 0 */
  /* add user code begin DebugMonitor_IRQ 1 */

  /* add user code end DebugMonitor_IRQ 1 */
}

/**
  * @brief  this function handles pendsv_handler exception.
  * @param  none
  * @retval none
  */
void PendSV_Handler(void)
{
  /* add user code begin PendSV_IRQ 0 */

  /* add user code end PendSV_IRQ 0 */
  /* add user code begin PendSV_IRQ 1 */

  /* add user code end PendSV_IRQ 1 */
}

/**
  * @brief  this function handles DMA1 Channel 1 handler.
  * @param  none
  * @retval none
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* add user code begin DMA1_Channel1_IRQ 0 */
  if(dma_interrupt_flag_get(DMA1_FDT1_FLAG) != RESET)
  {
    dma_flag_clear(DMA1_FDT1_FLAG);
		if(ADC_LOOP_DATA < FFT_LENGTH)
		{
			  ADC_Channel_0[ADC_LOOP_DATA] = ADC_DMA_data[0]>>4;
				ADC_Channel_1[ADC_LOOP_DATA] = ADC_DMA_data[1]>>4;
				ADC_Channel_2[ADC_LOOP_DATA] = ADC_DMA_data[2]>>4;
				ADC_Channel_3[ADC_LOOP_DATA] = ADC_DMA_data[3]>>4;
				ADC_Channel_4[ADC_LOOP_DATA] = ADC_DMA_data[4]>>4;
				ADC_Channel_5[ADC_LOOP_DATA] = ADC_DMA_data[5]>>4;
				ADC_Channel_6[ADC_LOOP_DATA] = ADC_DMA_data[6]>>4;
				ADC_LOOP_DATA++;
		}
  }
  /* add user code end DMA1_Channel1_IRQ 0 */
  /* add user code begin DMA1_Channel1_IRQ 1 */

  /* add user code end DMA1_Channel1_IRQ 1 */
}
/**
  * @brief  this function handles TMR3 handler.
  * @param  none
  * @retval none
  */
void TMR3_GLOBAL_IRQHandler(void)
{
  /* add user code begin TMR3_GLOBAL_IRQ 0 */
	if(tmr_flag_get(TMR3,TMR_OVF_FLAG) != RESET)
		{
	    tmr_flag_clear(TMR3,TMR_OVF_FLAG);
			adc_ordinary_software_trigger_enable(ADC1, TRUE);
		}

  //  adc_ordinary_channel_set(ADC1,ADC_CHANNEL_6,1,ADC_SAMPLETIME_7_5);
  //  adc_ordinary_software_trigger_enable(ADC1, TRUE);
  /* add user code end TMR3_GLOBAL_IRQ 0 */
  /* add user code begin TMR3_GLOBAL_IRQ 1 */

  /* add user code end TMR3_GLOBAL_IRQ 1 */
}
/**
  * @brief  this function handles TMR6 handler.
  * @param  none
  * @retval none
  */
void TMR6_GLOBAL_IRQHandler(void)
{
  /* add user code begin TMR6_GLOBAL_IRQ 0 */
  if(tmr_flag_get(TMR6,TMR_OVF_FLAG) != RESET)
		{
			table_bits_refresh();
			table_data++;
	    if(table_data == 18)table_data = 0;
			tmr_flag_clear(TMR6,TMR_OVF_FLAG);
		}
  /* add user code end TMR6_GLOBAL_IRQ 0 */
  /* add user code begin TMR6_GLOBAL_IRQ 1 */

  /* add user code end TMR6_GLOBAL_IRQ 1 */
}

/**
  * @brief  this function handles TMR14 handler.
  * @param  none
  * @retval none
  */
void TMR14_GLOBAL_IRQHandler(void)
{
  /* add user code begin TMR14_GLOBAL_IRQ 0 */

  /* add user code end TMR14_GLOBAL_IRQ 0 */
  /* add user code begin TMR14_GLOBAL_IRQ 1 */

  /* add user code end TMR14_GLOBAL_IRQ 1 */
}

/* add user code begin 1 */

/* add user code end 1 */
