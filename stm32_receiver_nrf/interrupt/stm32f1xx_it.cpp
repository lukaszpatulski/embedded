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

void NMI_Handler(void)
{
}


void HardFault_Handler(void)
{
	while(1) {
	}
  /* Go to infinite loop when Hard Fault exception occurs */
}


void EXTI2_IRQHandler(void)
{
	if((EXTI->IMR & EXTI_IMR_MR2) && (EXTI->PR & EXTI_PR_PR2))
		
	{
	  osSignalSet(CommunicationThread::id_thread, RTX_thread::NEW_DATA_INTERRUPT);

		//Set pending register
	  EXTI->PR |= EXTI_PR_PR2 ;
	}
}

void RTCAlarm_IRQHandler(void)
{
	if (RTC->CRH & RTC_CRL_ALRF)
	{
		osSignalSet(AlarmThread::id_thread, RTX_thread::ALARM_INTERRUPT);
		
		/* This bit is set by hardware when the 32-bit programmable counter reaches the threshold set 
			 in the RTC_ALR register. An interrupt is generated if ALRIE=1 in the RTC_CRH register. It
			 can be cleared only by software. Writing ‘1’ has no effect. */
		
		/* Clear the corresponding RTC pending bit */
		RTC->CRL &= (uint16_t)~RTC_CRL_ALRF;

		//Set pending register
	  EXTI->PR |= EXTI_PR_PR17 ;

		/* Set immediately alarm to obtain the best accuracy */
		setAlarm(AlarmThread::alarmValue);
  }
}

void ADC1_2_IRQHandler (void)
{
	
	if(ADC1->SR & ADC_SR_EOC)
  {
		osSignalSet(ExecutorThread::id_thread, RTX_thread::ADC_INTERRUPT);
		ADC1->SR &= ~ADC_SR_EOC;
  }
}

void ADC1_IRQHandler (void)
{
if(ADC1->SR & ADC_SR_EOC)
  {
  
		
  }
}
