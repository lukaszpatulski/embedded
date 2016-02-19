#include "main.h"
#include <stdbool.h>

/* Processor state */
sleepState processorState = RUN_MODE;

/* Extern objects */
LedBlinking ledManager;
NRF24L01_manager NRF24L01_manager_object;

// Thread functions
static void osReceiverNRF24L01_Task(void const *arg);
static void osAlarm_Task(void const *arg);
static void ledBlinking(void const *arg);

/* RTX tasks */
osThreadId ledTask;
osThreadDef(osReceiverNRF24L01_Task, osPriorityNormal, 1, 0); /* osThreadDef(name, priority, instances, stacksz) */

osThreadId osAlarmTask;
osThreadDef(osAlarm_Task, osPriorityBelowNormal, 1, 0); /* osThreadDef(name, priority, instances, stacksz) */

/* Virtual timer */
osTimerDef(timer_handle_0, ledBlinking);


int main (void) {
	
	/* Enable RTC */
	enableRTC();
	setAlarm(5);
	/* Initialize CMSIS-RTOS */
  osKernelInitialize ();
	
	/* Enable stop mode */
	sleepConfig();

	/* Led menager initialization */
	ledManager.init();
	//ledManager.setExternal();
	
	/* NRF24L01 Initialization */
	NRF24L01_manager_object.InitializeNRF24L01();
		
  /* Create a thread and add it to Active Threads and set it to state READY */
  /* Receiver thread create. Passed argument to function: NULL */
  ledTask = osThreadCreate (osThread(osReceiverNRF24L01_Task), NULL);
	
	/* Create a thread and add it to Active Threads and set it to state READY */
  /* Alarm thread create. Passed argument to function: NULL */
  osAlarmTask = osThreadCreate (osThread(osAlarm_Task), NULL);

	/* Create virtual timer */
	osTimerId timer0 = osTimerCreate(osTimer(timer_handle_0), osTimerPeriodic, (void *)0);
	/* Start timer */
	osTimerStart (timer0, 70);

	/* Start threads execution */
  osKernelStart ();
}

static void osReceiverNRF24L01_Task(void const *arg)  // USE MUTEX !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
	while(1) {
		
		osSignalWait(0x01,osWaitForever);	
		
		/* I the CPU was awakened */
		if (processorState == STOP_MODE)
		{
			restoreClockPower();
		}
		
		ledManager.resetRed();
		uint8_t dataIn[32];
		if (NRF24L01_manager_object.getData(dataIn))
		{
			if(!strncmp( (char*)dataIn, "start", 5 )) 
			{
				osDelay(2000);
			}
		}

		uint8_t dataOut[32] = "sth!!!!";
		/* Transmit data */
		NRF24L01_manager_object.NRF24L01_Transmit(dataOut);
		/* Wait for data to be sent */
		NRF24L01_Transmit_Status_t transmissionStatus;
		do 
		{
				transmissionStatus = NRF24L01_manager_object.NRF24L01_GetTransmissionStatus();
		} while (transmissionStatus == NRF24L01_Transmit_Status_Sending);
		/* Go to RX mode */
		NRF24L01_manager_object.NRF24L01_PowerUpRx();
		
		ledManager.setRed();
		ledManager.resetExternal();
		goToSleep();
	}
}

static void osAlarm_Task(void const *arg) // USE MUTEX !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
	while(1) 
	{

		osSignalWait(0x02,osWaitForever);
		
		/* I the CPU was awakened */
		if (processorState == STOP_MODE)
		{
			restoreClockPower();
		}
		
		ledManager.setExternal();

		osDelay(2000);
		ledManager.resetExternal();
		goToSleep();

	}
}
	


static void ledBlinking(void const *arg) {
	
	static bool ledToggle = true;

	if (ledToggle) {
		ledManager.setGreen();
		ledToggle = false;
	}
	else {
		ledManager.resetGreen();
		ledToggle = true;
	}
}	

