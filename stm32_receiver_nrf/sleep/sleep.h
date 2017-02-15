#ifndef __sleep
#define __sleep

#include <stm32f10x.h>

void sleepConfig(void);
void goToSleep(void);
void restoreClockPower(void);

void enableRTC(void);
void setAlarm(uint32_t AlarmValue = 10);

typedef enum {
	
	STOP_MODE,
	RUN_MODE

} sleepState;

extern sleepState processorState;

#endif
