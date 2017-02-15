#ifndef OS_THREAD
#define OS_THREAD

#include "cmsis_os.h"

#include "leds.h"
#include "os_queue.h"
#include "os_mutex.h"
#include "nrf24L01.h"
#include "sleep.h"
#include "adc.h"
#include "flash.h"

class RTX_thread
{
	private:
		
		osThreadDef_t instance_thread;
	
	public:
		
		enum 
		{
			NEW_DATA_INTERRUPT = 1,
			ALARM_INTERRUPT    = 2,
			ADC_INTERRUPT      = 3,
			DIAG_FRAME         = 21,
      EXPANDER_FRAME     = 22,
			VOLTAGE_REQ        = 23,
      ALARM_REQ          = 24
			
		};

		RTX_thread(os_pthread callbackFunc, osPriority priority, uint32_t instances, uint32_t stacksize, osThreadId &threadId);
		
		static bool executionLog;
};

/* ======================= Custom classes ============================ */

class CommunicationThread: public RTX_thread
{
	public:

		static osThreadId id_thread;

		static void collback_function(void const *arg); // Customize only callback function

		CommunicationThread(osPriority priority, uint32_t instances, uint32_t stacksize): 
		RTX_thread(collback_function, priority, instances, stacksize, id_thread) {}
			
	  operator bool() const { return (id_thread != NULL); }
		bool operator!() const {return (id_thread == NULL); }

};

class ExecutorThread: public RTX_thread
{
	public:

		static osThreadId id_thread;

		static void collback_function(void const *arg); // Customize only callback function

		ExecutorThread(osPriority priority, uint32_t instances, uint32_t stacksize): 
		RTX_thread(collback_function, priority, instances, stacksize, id_thread) {}
			
	  operator bool() const { return (id_thread != NULL); }
		bool operator!() const {return (id_thread == NULL); }

};

class AlarmThread: public RTX_thread
{
	public:
		
		static uint32_t alarmValue;

		static osThreadId id_thread;

		static void collback_function(void const *arg); // Customize only callback function

		AlarmThread(osPriority priority, uint32_t instances, uint32_t stacksize): 
		RTX_thread(collback_function, priority, instances, stacksize, id_thread) {}
			
	  operator bool() const { return (id_thread != NULL); }
		bool operator!() const {return (id_thread == NULL); }

};

class SleeperThread: public RTX_thread
{
	public:

		static osThreadId id_thread;

		static void collback_function(void const *arg); // Customize only callback function

		SleeperThread(osPriority priority, uint32_t instances, uint32_t stacksize): 
		RTX_thread(collback_function, priority, instances, stacksize, id_thread) {}
			
	  operator bool() const { return (id_thread != NULL); }
		bool operator!() const {return (id_thread == NULL); }

};

#endif
