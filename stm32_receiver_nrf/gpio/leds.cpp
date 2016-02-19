#include "leds.h"


LedBlinking::LedBlinking(void) {

	Led_Green_and_Red_Leds.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
	Led_Green_and_Red_Leds.GPIO_Speed = GPIO_Speed_2MHz;
	Led_Green_and_Red_Leds.GPIO_Mode  = GPIO_Mode_Out_PP;
	
	External_led.GPIO_Pin = GPIO_Pin_1;
	External_led.GPIO_Speed = GPIO_Speed_2MHz;
	External_led.GPIO_Mode  = GPIO_Mode_Out_PP;
	
}

void LedBlinking::init(void) {
	
	/* Enable GPIO_B clock */
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	
	/* Enable GPIO_A clock */
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	GPIO_Init(GPIOB, &Led_Green_and_Red_Leds);
	GPIO_Init(GPIOA, &External_led);
}

void LedBlinking::setGreen(void) {
	
	GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

void LedBlinking::setRed(void) {
	
	GPIO_SetBits(GPIOB, GPIO_Pin_1);
}

void LedBlinking::resetGreen(void) {
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

void LedBlinking::resetRed(void) {
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
}

void LedBlinking::setExternal(void) {
	
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void LedBlinking::resetExternal(void) {
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}
