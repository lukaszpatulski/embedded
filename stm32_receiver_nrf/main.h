#ifndef __main
#define __main

#include <stm32f10x.h>
#include "stm32f10x_gpio.h"
#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "leds.h"
#include "nrf24l01.h"
#include "cmsis_os.h"
#include "sleep.h"

/* Thread ids */
extern osThreadId ledTask;
extern osThreadId osAlarmTask;

#endif

