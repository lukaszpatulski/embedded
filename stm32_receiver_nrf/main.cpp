#include "main.h"
#include <stdbool.h>

/* Processor state */
sleepState processorState = RUN_MODE;

/* Peripherial objects */
LedBlinking ledManager;
NRF24L01_manager NRF24L01_manager_object;
ADC_battery adcBatteryManager;
Flash alarmValueFlash;


/* Queue */
Queue os_queue = Queue();

/* Mutex */
RtcMutex os_rtxClockMutex = RtcMutex();


int main (void) {

	
	/* ENABLE or DISABLE debug in stop mode */
	DBGMCU_Config(DBGMCU_STOP, ENABLE);
	
	/* ----------------- Enable RTC --------------------- */
	enableRTC();
	// Set alarm (seconds)
	setAlarm(AlarmThread::alarmValue);

	/* Enable stop mode */
	sleepConfig();
	
	/* ---------------- Init peripherials --------------- */
	
	/* Initialization ADC */
	adcBatteryManager.initAdc();

	/* Led menager initialization */
	ledManager.init();
	//ledManager.setExternal();
	
	/* NRF24L01 Initialization */
	NRF24L01_manager_object.InitializeNRF24L01();

	
	
	/* ============================= RTOS RTX =============================================== */
	
	/* Initialize CMSIS-RTOS */
  osKernelInitialize ();

/* ----------------------- Threads (arg: priority, num instances, stack size) ------------- */
	CommunicationThread os_communicationThread = CommunicationThread(osPriorityAboveNormal, 1, 512);
	
	ExecutorThread os_executorThread = ExecutorThread(osPriorityNormal, 1, 0);
	
	AlarmThread os_alarmThread = AlarmThread(osPriorityNormal, 1, 0);
	
	SleeperThread os_sleeperThread = SleeperThread(osPriorityLow, 1, 0);
	
	/* ----------------------- Timers (arg: timer type) ------------------------------------- */
	RTX_timer_blinker os_blinkerTimer = RTX_timer_blinker(osTimerPeriodic);
	
	
	
	/* ++++++++++++++++++ Check initialization of os objects +++++++++ */
	
	if (!(os_communicationThread && os_executorThread && os_alarmThread && os_rtxClockMutex && os_queue && os_blinkerTimer && os_sleeperThread))
	{
		ledManager.setRed();
	}
	
	/* ------------------------- Start threads execution ------------- */
  osKernelStart ();
	
	/* ------------------------ Release mutex ------------------------------- */
	os_rtxClockMutex.releaseMutex();
	
	/* ------------------------ Timer start ------------------------- */
	os_blinkerTimer.start_timer(300);
}
