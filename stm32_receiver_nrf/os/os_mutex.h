#ifndef OS_MUTEX
#define OS_MUTEX

#include "cmsis_os.h"

class RtcMutex
{
	private:

		osMutexDef_t instance_mutex;
		static uint32_t os_mutex_storage[4];
	
		static osMutexId id_mutex;
	
	public:

		RtcMutex(void);
	
	  operator bool() const { return (id_mutex != NULL); }
		bool operator!() const {return (id_mutex == NULL); }
		
		bool waitForMutex(void);
		
		bool releaseMutex(void);
};

#endif