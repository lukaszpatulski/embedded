#include "main.h"
#include <stdbool.h>


typedef struct {
uint8_t dataToSend[32];

} memory_block_dataToSend_t;

/* Processor state */
sleepState processorState = RUN_MODE;

/* Extern objects */
LedBlinking ledManager;
NRF24L01_manager NRF24L01_manager_object;
ADC_battery adcBatteryManager;

/* Mutexes */
osMutexId enableClockMutex;
osMutexDef(enableClockMutex);

/* Messages */
osMessageQId Q_dataSender;		
osMessageQDef (Q_dataSender, 3, uint32_t );	// name, queue_sz, type 

// Thread functions
static void osReceiverNRF24L01_Task(void const *arg);
static void osAlarm_Task(void const *arg);
static void ledBlinking(void const *arg);
static void osSender(void const *arg);

/* Variables */
uint16_t lastADCvalue = 0;

/* RTX tasks */
osThreadId ledTask;
osThreadDef(osReceiverNRF24L01_Task, osPriorityNormal, 1, 512); /* osThreadDef(name, priority, instances, stacksz(in bytes)) */

osThreadId osSender_id;
osThreadDef(osSender, osPriorityBelowNormal, 1, 0); /* osThreadDef(name, priority, instances, stacksz = default ) */

osThreadId osAlarmTask;
osThreadDef(osAlarm_Task, osPriorityBelowNormal, 1, 0); /* osThreadDef(name, priority, instances, stacksz = default ) */

/* Virtual timer */
osTimerDef(timer_handle_0, ledBlinking);


int main (void) {
	
	/* ENABLE or DISABLE debug in stop mode */
	DBGMCU_Config(DBGMCU_STOP, DISABLE);
	
	/* Enable RTC */
	enableRTC();
	setAlarm(15);
	
	/* Initialize CMSIS-RTOS */
  osKernelInitialize ();
	
	/* Enable stop mode */
	sleepConfig();
	
	/* Initialization ADC */
	adcBatteryManager.initAdc();

	/* Led menager initialization */
	ledManager.init();
	//ledManager.setExternal();
	
	/* NRF24L01 Initialization */
	NRF24L01_manager_object.InitializeNRF24L01();
	
	/* Create mutex */
	enableClockMutex = osMutexCreate(osMutex(enableClockMutex));
	
	/* create the message queue */
	Q_dataSender = osMessageCreate(osMessageQ(Q_dataSender),NULL);					
		
  /* Create a thread and add it to Active Threads and set it to state READY */
  /* Receiver thread create. Passed argument to function: NULL */
  ledTask = osThreadCreate (osThread(osReceiverNRF24L01_Task), NULL);
	
	/* Create a thread and add it to Active Threads and set it to state READY */
  /* Alarm thread create. Passed argument to function: NULL */
  osAlarmTask = osThreadCreate (osThread(osAlarm_Task), NULL);
	
	/* Create a thread and add it to Active Threads and set it to state READY */
  /* Alarm thread create. Passed argument to function: NULL */
  osSender_id = osThreadCreate (osThread(osSender), NULL);

	/* Create virtual timer */
	osTimerId timer0 = osTimerCreate(osTimer(timer_handle_0), osTimerPeriodic, (void *)0);
	/* Start timer */
	osTimerStart (timer0, 70);

	/* Start threads execution */
  osKernelStart ();
}

static void osReceiverNRF24L01_Task(void const *arg)
{
	while(1) {
		
		osSignalWait(0x01,osWaitForever);	

		/* Secure initialization of system clock */
		osMutexWait(enableClockMutex, osWaitForever);  
		/* I the CPU was awakened */
		if (processorState == STOP_MODE)
		{
			restoreClockPower();
		}
		osMutexRelease(enableClockMutex);
		
		ledManager.resetRed();
		uint8_t dataIn[32];
		if (NRF24L01_manager_object.getData(dataIn))
		{
			if(!strncmp( (char*)dataIn, "start", 5 )) 
			{
				char buffer[32];
				uint8_t dataOut[32];

				sprintf (buffer, "%d mV", lastADCvalue);
				memcpy(dataOut, buffer, 32 * sizeof(char));

				// Add o queue 
				osMessagePut(Q_dataSender, (uint32_t)dataOut, osWaitForever); 
				osDelay(2000);
			}
		}

		
		
		ledManager.setRed();
		ledManager.resetExternal();
		goToSleep();
	}
}

static void osAlarm_Task(void const *arg)
{
	while(1) 
	{

		osSignalWait(0x02,osWaitForever); // Wait for alarm interrupt
		
		/* Secure initialization of system clock */
		osMutexWait(enableClockMutex, osWaitForever); 
		/* I the CPU was awakened */
		if (processorState == STOP_MODE)
		{
			restoreClockPower();
		}
		osMutexRelease(enableClockMutex);

		adcBatteryManager.ADC_startConversion();
		osSignalWait(0x03,osWaitForever); // Wait for end of conversion
		lastADCvalue = adcBatteryManager.ADC_GetConversionValue();
		//ledManager.setExternal();
		osDelay(4000);
		//ledManager.resetExternal();
		goToSleep();

	}
}

static void osSender(void const *arg) 
{
	
	osEvent  result_dataToSend;
	while(1) 
	{
		result_dataToSend = osMessageGet(Q_dataSender, osWaitForever);				//wait for a message to arrive
		
			ledManager.setExternal();

			/* Transmit data */
			NRF24L01_manager_object.NRF24L01_Transmit((uint8_t *)result_dataToSend.value.v);
			/* Wait for data to be sent */
			NRF24L01_Transmit_Status_t transmissionStatus;
			do 
			{
					transmissionStatus = NRF24L01_manager_object.NRF24L01_GetTransmissionStatus();
			} while (transmissionStatus == NRF24L01_Transmit_Status_Sending);
				{
					osDelay(5);
				}
			/* Go to RX mode */
			NRF24L01_manager_object.NRF24L01_PowerUpRx();
		  ledManager.resetExternal();
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

