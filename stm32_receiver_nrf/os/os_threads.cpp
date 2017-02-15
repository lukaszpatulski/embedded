#include "os_threads.h"

extern LedBlinking ledManager;
extern Queue os_queue;
extern RtcMutex os_rtxClockMutex;
extern NRF24L01_manager NRF24L01_manager_object;
extern sleepState processorState;
extern ADC_battery adcBatteryManager;
extern Flash alarmValueFlash;

/* Log any execution of thread */
bool RTX_thread::executionLog = true;

uint16_t alarmFlash;

NRF24L01_Transmit_Status_t transmissionStatus;


RTX_thread::RTX_thread(os_pthread callbackFunc, osPriority priority, uint32_t instances, uint32_t stacksize, osThreadId &threadId)
{
	instance_thread.pthread   = callbackFunc;
	instance_thread.tpriority = priority;
	instance_thread.instances = instances;
	instance_thread.stacksize = stacksize;
	
	threadId = osThreadCreate( &instance_thread, NULL );
}


/* ======================= Custom classes ============================ */

osThreadId CommunicationThread::id_thread = NULL;
osThreadId ExecutorThread::id_thread = NULL;
osThreadId AlarmThread::id_thread = NULL;
osThreadId SleeperThread::id_thread = NULL;

uint32_t AlarmThread::alarmValue = 20;


void CommunicationThread::collback_function(void const *arg)
{
	uint8_t dataIn[4] = {0};

	while(1) 
	{
		osSignalWait(NEW_DATA_INTERRUPT, osWaitForever);
		
		/* Log execution */
		RTX_thread::executionLog = true;
		
		/* Secure initialization of system clock */
		os_rtxClockMutex.waitForMutex();  
		/* I the CPU was awakened */
		if (processorState == STOP_MODE)
		{
			restoreClockPower();
		}
		os_rtxClockMutex.releaseMutex();
		
		/* Gets status senfing FF and checks specific bits */
		if (NRF24L01_manager_object.NRF24L01_DataReady()) 
		{
			/* Get data from NRF24L01+ */
			NRF24L01_manager_object.NRF24L01_GetData(dataIn);
			/* Push data to queue */
			os_queue.send(dataIn[0], dataIn[1], dataIn[2], dataIn[3]);
		}
	}
}

void ExecutorThread::collback_function(void const *arg)
{
	uint8_t receiv[4] = {0};
	uint8_t send[4]   = {0};

	while(1) 
	{
		/* Pop data from queue */
		os_queue.receive(receiv);
		
		/* Log execution */
		RTX_thread::executionLog = true;
		
		switch( receiv[0] )
		{
			case DIAG_FRAME:

				if(receiv[1] == ALARM_REQ)
				{				
					alarmFlash = alarmValueFlash.readFlash();
					alarmValueFlash.write((uint16_t)34);
					alarmFlash = alarmValueFlash.readFlash();
					
					
					AlarmThread::alarmValue = ((uint32_t)receiv[2] << 8) | (uint32_t)receiv[3];
					setAlarm( AlarmThread::alarmValue );
				}
				if(receiv[1] == VOLTAGE_REQ)
				{
					adcBatteryManager.ADC_startConversion();
					osSignalWait(RTX_thread::ADC_INTERRUPT,osWaitForever); // Wait for end of conversion
					uint16_t ADCvalue = adcBatteryManager.ADC_GetConversionValue();
					
					/* Preprare frame to send */
					send[0] = DIAG_FRAME;
					send[1] = VOLTAGE_REQ;
					send[2] = (ADCvalue & 0xFF00) >> 8;
					send[3] = ADCvalue & 0x00FF;
 					
					/* Transmit data */
					NRF24L01_manager_object.NRF24L01_Transmit(send);
					/* Wait for data to be sent */
					//NRF24L01_Transmit_Status_t transmissionStatus;
					uint8_t timeOutSending = 5;
					do 
					{
						transmissionStatus = NRF24L01_manager_object.NRF24L01_GetTransmissionStatus();
						osDelay(5);
						timeOutSending -= 1;
						if (timeOutSending < 1)
						{
							// Sending failed
							break;
						}
						
					} while (transmissionStatus == NRF24L01_Transmit_Status_Sending); // Wait for end of transmition

					/* Go to RX mode */
					NRF24L01_manager_object.NRF24L01_PowerUpRx();
				
				}
				// Bink green led
				ledManager.setGreen();
				osDelay(50);
				ledManager.resetGreen();

				break;
    
			case EXPANDER_FRAME:
				ledManager.setGreen();
				osDelay(50);
				ledManager.resetGreen();
				osDelay(50);
				ledManager.setGreen();
				osDelay(50);
				ledManager.resetGreen();
				osDelay(50);
				ledManager.setGreen();
				osDelay(50);
				ledManager.resetGreen();
				break;
					
			default:
				ledManager.setGreen();
				osDelay(100);
				ledManager.resetGreen();
				break;
		}
	}
}


void AlarmThread::collback_function(void const *arg)
{
	while(1) 
	{
		osSignalWait(ALARM_INTERRUPT, osWaitForever);
		
		/* Log execution */
		RTX_thread::executionLog = true;

		/* Secure initialization of system clock */
		os_rtxClockMutex.waitForMutex();  
		/* I the CPU was awakened */
		if (processorState == STOP_MODE)
		{
			restoreClockPower();
		}
		os_rtxClockMutex.releaseMutex();

	}
}


void SleeperThread::collback_function(void const *arg)
{
	while(1) 
	{
		if(RTX_thread::executionLog == true)
		{
			RTX_thread::executionLog = false;
			osDelay(500);
			if (RTX_thread::executionLog == false)
			{
				goToSleep();
				RTX_thread::executionLog = true;
			}
		}
	}
}