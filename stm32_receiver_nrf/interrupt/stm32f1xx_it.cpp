/**
  ******************************************************************************
  * @file    IO_Toggle/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "stm32f1xx_it.h"
/** @addtogroup STM32F0_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup IO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t dataIn[32];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	while(1) {
	}
  /* Go to infinite loop when Hard Fault exception occurs */
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */
void EXTI2_IRQHandler(void)
{
	if((EXTI->IMR & EXTI_IMR_MR2) && (EXTI->PR & EXTI_PR_PR2))
		
	{
		do {
			if(NRF24L01_manager_object.ReceiveData() )
			{
				osSignalSet(ledTask, 0x01);
			}	
		}
		while(!NRF24L01_manager_object.NRF24L01_Empty_RX_Fifo());
		//Set pending register
	 EXTI->PR |= EXTI_PR_PR2 ;
	}
}

void RTCAlarm_IRQHandler(void)
{
	if (RTC->CRH & RTC_CRL_ALRF)
	{
		osSignalSet(osAlarmTask, 0x02);
		
		/* This bit is set by hardware when the 32-bit programmable counter reaches the threshold set 
			 in the RTC_ALR register. An interrupt is generated if ALRIE=1 in the RTC_CRH register. It
			 can be cleared only by software. Writing ‘1’ has no effect. */
		
		/* Clear the corresponding RTC pending bit */
		RTC->CRL &= (uint16_t)~RTC_CRL_ALRF;

		//Set pending register
	  EXTI->PR |= EXTI_PR_PR17 ;

		/* Set immediately alarm to obtain the best accuracy */
		setAlarm(10);
  }
}

void ADC1_2_IRQHandler (void)
{
	
	if(ADC1->SR & ADC_SR_EOC)
  {
		osSignalSet(osAlarmTask, 0x03);
		ADC1->SR &= ~ADC_SR_EOC;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
