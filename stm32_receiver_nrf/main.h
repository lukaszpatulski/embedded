#ifndef __main
#define __main

#include <stdio.h>
#include <stm32f10x.h>
#include "stm32f10x_gpio.h"
#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "leds.h"
#include "nrf24l01.h"
#include "cmsis_os.h"
#include "sleep.h"
#include "adc.h"
#include "stm32f10x_dbgmcu.h"
#include "stm32f10x_i2c.h"
#include "flash.h"

#include "os_timer.h"
#include "os_threads.h"
#include "os_queue.h"
#include "os_mutex.h"


#endif

