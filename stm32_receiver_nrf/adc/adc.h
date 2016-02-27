#ifndef __adc
#define __adc 20

#include "stm32f10x_gpio.h"

class ADC_battery
{
	
public:

	void initAdc(void);
	uint16_t ADC_GetConversionValue(void);
	void ADC_startConversion(void);

};


#endif
