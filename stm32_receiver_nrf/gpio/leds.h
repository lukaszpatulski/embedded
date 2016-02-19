#ifndef __leds
#define __leds

#include "stm32f10x_gpio.h"

class LedBlinking
{
public:
	
	LedBlinking();
		
  void setGreen(void);
  void setRed(void);

	void resetGreen(void);
  void resetRed(void);

  void setExternal(void);
  void resetExternal(void);

	void init(void);

private:
	
	GPIO_InitTypeDef Led_Green_and_Red_Leds;
  GPIO_InitTypeDef External_led;

};

extern LedBlinking ledManager;

#endif
