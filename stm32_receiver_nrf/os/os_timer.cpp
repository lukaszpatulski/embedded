#include "os_timer.h"
#include "leds.h"

extern LedBlinking ledManager;
   	
osTimerId RTX_timer_blinker::id_timer = NULL;		
uint32_t RTX_timer_blinker::os_timer_storage[6] = {0};

RTX_timer_blinker::RTX_timer_blinker(os_timer_type type)
{
	instance_timer.ptimer = collback_function;
	instance_timer.timer = os_timer_storage;

	id_timer = osTimerCreate(&instance_timer, type, NULL);
}

bool RTX_timer_blinker::start_timer(int delay)
{
	osStatus status = osTimerStart (id_timer, delay);
	if (status == osOK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool RTX_timer_blinker::stop_timer(void)
{
	osStatus status = osTimerStop(id_timer);
	if (status == osOK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void RTX_timer_blinker::collback_function (void const *arg)
{
		ledManager.setRed();
	  osDelay(50);
		ledManager.resetRed();
}