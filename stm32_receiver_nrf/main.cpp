#include "main.h"
#include <stdbool.h>

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
osMessageQDef (Q_dataSender, 10, uint32_t );	// name, queue_sz, type 

/* Memory pool */
osPoolDef(osMessageTask, 10, uint32_t); //name, max number in memory pool, type

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

void I2C_Setup(void);
void set_output(void);


int main (void) {
	
	/* ENABLE or DISABLE debug in stop mode */
	DBGMCU_Config(DBGMCU_STOP, ENABLE);
	
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
	
	////////////////////////
	//I2C_Setup();
	//set_output();
	/////////////////////////
	
	
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
				
				/* Allocate memory for queue */
				osPoolId osMessageTask_ID;
				osMessageTask_ID = osPoolCreate (osPool (osMessageTask));
				if (osMessageTask_ID != NULL)  {
					
					// allocate a memory block
					uint32_t *addr;
					addr = (uint32_t *)osPoolAlloc (osMessageTask_ID);
					
					if (addr != NULL) {
						/* Memory block was allocated
						   Add to queue pointer */
						osMessagePut(Q_dataSender, (uint32_t)dataOut, osWaitForever); 
						osDelay(2000);
					}
				}

				
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

void I2C_Setup(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /*enable I2C*/
    I2C_Cmd(I2C1,ENABLE);
	
	  /* I2C1 clock enable */
	  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	
	
    /* I2C1 SDA and SCL configuration - PB6 SCL, PB7 SDA*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* I2C1 configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_Init(I2C1, &I2C_InitStructure);

}

void set_output(void)
{

    /* initiate start sequence */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* check start bit flag */
    while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));
    /*send write command to chip*/
    I2C_Send7bitAddress(I2C1, (0x40), I2C_Direction_Transmitter);
    /*check master is now in Tx mode*/
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    /*set pins*/
    I2C_SendData(I2C1, 0x0F);
    /*wait for byte send to complete*/
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    /*generate stop*/
    I2C_GenerateSTOP(I2C1, ENABLE);
    /*stop bit flag*/
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));

}

