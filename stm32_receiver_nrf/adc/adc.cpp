#include "adc.h"


void ADC_battery::initAdc(void)
{
	/* Enable GPIO_A clock */
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	/* PA3, ADC1 IN3 */
	GPIO_InitTypeDef ADC_pin;
	ADC_pin.GPIO_Pin   = GPIO_Pin_3;
	ADC_pin.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &ADC_pin);
	
	/* The ADC input clock is generated from the PCLK2 clock divided by a prescaler and it must
		not exceed 14 MHz */
	/* ADC prescaler = 10: PCLK2 divided by 6 (72/6)*/
	RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;
	
	/* Enable clock for ADC 1 */
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	/* Single conversion mode */
	ADC1->CR2 &= ~(uint32_t)ADC_CR2_CONT;
	/*!< A/D Converter ON */
	ADC1->CR2 |= ADC_CR2_ADON;
	/* Sample time = 239.5 cycles (first bit set) */
	ADC1->SMPR2 |= ADC_SMPR2_SMP3_1 | ADC_SMPR2_SMP3_0 | ADC_SMPR2_SMP3_2;
	/* ADC1 regular channel sequence length = 1 conversion */
	ADC1->SQR1 &= ~(uint32_t)ADC_SQR1_L;
	/* Channel 3 first in sequence */
	ADC1->SQR3 |= ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1;
	
	/* External event selected for regular group = software */
	ADC1->CR2 |= ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2;
	
	/*!< Interrupt enable for EOC */
	ADC1->CR1 |= ADC_CR1_EOCIE;
	//ADC_SR_EOC; 
	
	/* Set priority = 1 */
	NVIC_SetPriority(ADC1_2_IRQn, 1);
	/* Enable interrupt */
  NVIC_EnableIRQ(ADC1_2_IRQn);

}

void ADC_battery::ADC_startConversion(void)
{
	ADC1->CR2 |= ADC_CR2_ADON;
	/*!< Start Conversion of regular channels */
	ADC1->CR2 |= ADC_CR2_SWSTART;
}

uint16_t ADC_battery::ADC_GetConversionValue(void)
{
  /* Return the selected ADC conversion value */
  return (uint16_t) ADC1->DR;
}

	
	


