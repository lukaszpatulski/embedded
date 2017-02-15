#include "sleep.h"
#include "flash.h"

extern Flash alarmValueFlash;

void goToSleep(void) {
	
	processorState = STOP_MODE;
	__wfi();
}

void sleepConfig(void) {

	/* When an event or interrupt enters pending state, the event signal wakes up the processor from
	WFE. If the processor is not waiting for an event, the event is registered and affects the next WFE.
	The processor also wakes up on execution of an SEV instruction or an external event
	0: Only enabled interrupts or events can wakeup the processor, disabled interrupts are excluded
	1: Enabled events and all interrupts, including disabled interrupts, can wakeup theprocessor */
	SCB->SCR &= (int32_t)(~(int32_t)SCB_SCR_SEVONPEND_Msk);
	
	/* Controls whether the processor uses sleep or deep sleep as its low power mode:
  0: Sleep
  1: Deep sleep. */
	SCB->SCR |= ((int32_t)SCB_SCR_SLEEPDEEP_Msk);
	
	/* Configures sleep-on-exit when returning from Handler mode to Thread mode. Setting this bit to
	1 enables an interrupt-driven application to avoid returning to an empty main application.
	0: Do not sleep when returning to Thread mode.
	1: Enter sleep, or deep sleep, on return from an interrupt service routine. */
	SCB->SCR &= (int32_t)(~(int32_t)SCB_SCR_SLEEPONEXIT_Msk);

}

void restoreClockPower(void) 
{
	/* Wait until HSI ready */
	while ((RCC->CR & RCC_CR_HSIRDY) == 0) {}
	/* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
	/* Wait until HSE ready */
  while((RCC->CR & RCC_CR_HSERDY) == 0) {}
	/* Disable main PLL */
	RCC->CR &= ~(RCC_CR_PLLON);

  /* 
	 * FLASH configuration block
	 * enable instruction cache
	 * enable prefetch
	 * set latency to:
	
	   - zero wait state, if 0 < SYSCLK = 24 MHz
     - one wait state, if 24 MHz < SYSCLK = 48 MHz
     - two wait states, if 48 MHz < SYSCLK = 72 MHz
	
	 Enable Prefetch Buffer */
   FLASH->ACR |= FLASH_ACR_PRFTBE;

   /* Flash 2 wait state */
   FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
   FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2; 

   /* AHB prescaler */
   RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
   /*  APB high-speed prescaler (APB2) */
   RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
   /*  APB low-speed prescaler (APB1) */
   RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;	
	 /*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz and HSE as source for PLL*/
   RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));
   RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);
	 /* Enable PLL */
   RCC->CR |= RCC_CR_PLLON;
   /* Wait till PLL is ready */
   while((RCC->CR & RCC_CR_PLLRDY) == 0){}
	 /* Select PLL as system clock source */
   RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
   RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL; 
		 
	 /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08){}
			
		/* Set run mode */
		processorState = RUN_MODE;
}

void enableRTC(void)
{
	
	
	/* Power interface clock enable */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	/* Backup interface clock enable */
	RCC->APB1ENR |= RCC_APB1ENR_BKPEN;

	/* Access to RTC and Backup registers enabled */
	PWR->CR |= PWR_CR_DBP;
	
	/* Internal low-speed oscillator enable */
	RCC->CSR |= RCC_CSR_LSION;
	/* Wait till LSI is ready */
  while((RCC->CR & RCC_CSR_LSIRDY) == 0){}
	/* LSI oscillator clock used as RTC clock */
	RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;
	/* RTC clock enable */
	RCC->BDCR |= RCC_BDCR_RTCEN;
		
	/* ---- RTC REGISTERS ---- */	
		
	/* Clear RSF flag */
  RTC->CRL &= (uint16_t)~RTC_CRL_RSF;
  /* Loop until RSF flag is set */
  while ((RTC->CRL & RTC_CRL_RSF) == (uint16_t)RESET){}
		
	/* A new value can be written to the RTC registers only when the RTOFF status bit value is ’1’ */
	while((RTC->CRL & RTC_CRL_RTOFF) == 0){}
	/* This bit must be set by software to enter in configuration mode so as to allow new values to
		 be written in the RTC_CNT, RTC_ALR or RTC_PRL registers. The write operation is only
		 executed when the CNF bit is reset by software after has been set */
	RTC->CRL |= RTC_CRL_CNF;
		
	/* ------------------------------------- Operations rtc registers --------------------------------------- */	

	/* Alarm Interrupt Enable */
	RTC->CRH |= RTC_CRH_ALRIE;              
	/* OverfloW Interrupt Enable */	
	//RTC->CRH |= RTC_CRH_OWIE;  
		
	uint32_t PrescalerValue = 32768;

	RTC->PRLH = (PrescalerValue & 0x000F0000) >> 16;
  /* Set RTC PRESCALER LSB word */
  RTC->PRLL = (PrescalerValue & 0x0000FFFF);

	/* ------------------------------------- End --------------------------------------- */		

	/* Clear the CNF bit to exit configuration mode */
	RTC->CRL &= (uint16_t)~((uint16_t)RTC_CRL_CNF);
		
	/* Poll RTOFF, wait until its value goes to ‘1’ to check the end of the write operation. */
	while((RTC->CRL & RTC_CRL_RTOFF) == 0){}
		
		/* EXTI line 17 is connected to the RTC Alarm event */
	EXTI->IMR |= EXTI_IMR_MR17;
	/* Raising edge */
  EXTI->FTSR |= EXTI_RTSR_TR17;	
		
	/* Event mask line 17 */
	EXTI->EMR |= EXTI_EMR_MR17;
	
	/* Set priority = 1 */
	NVIC_SetPriority(RTCAlarm_IRQn, 15);
	/* Enable interrupt */
  NVIC_EnableIRQ(RTCAlarm_IRQn);
	
}

/* Set alaram in seconds */
void setAlarm(uint32_t AlarmValue)
{
	// If alarm value present in flash memory 
	if(alarmValueFlash.readFlash() != 0xFFFF)
	{
		AlarmValue = (uint32_t)alarmValueFlash.readFlash();
	}
	
	/* Get current RTC counter value */
	uint16_t tmp = 0;
  tmp = RTC->CNTL;
  uint32_t counterValue=  (((uint32_t)RTC->CNTH << 16 ) | tmp);
	
	if ((counterValue + AlarmValue) > 0xFFFFFFFF)
	{
		AlarmValue = AlarmValue - (0xFFFFFFFF - counterValue);
	}
	else 
	{
		AlarmValue += counterValue;
	}
	
	/* Set the CNF flag to enter in the Configuration Mode */
  RTC->CRL |= RTC_CRL_CNF;
	
	/* ------------------------------------- Operations rtc registers --------------------------------------- */	
	
	/* When the programmable counter reaches the 32-bit value stored in the RTC_ALR register,
		 an alarm is triggered and the RTC_alarmIT interrupt request is generated */
	
	/* Set the ALARM MSB word */
  RTC->ALRH = AlarmValue >> 16;
  /* Set the ALARM LSB word */
  RTC->ALRL = (AlarmValue & 0xFFFF);
	
	/* ------------------------------------- End --------------------------------------- */	
	
	/* Reset the CNF flag to exit from the Configuration Mode */
  RTC->CRL &= (uint16_t)~((uint16_t)RTC_CRL_CNF); 

	/* Loop until RTOFF flag is set */
  while ((RTC->CRL & RTC_CRL_RTOFF) == (uint16_t)RESET){}
	
}
