#include "os_mutex.h"

uint32_t RtcMutex::os_mutex_storage[4] = {0};
osMutexId RtcMutex::id_mutex = NULL;

RtcMutex::RtcMutex(void)
{
	instance_mutex.mutex = os_mutex_storage;
	
	id_mutex = osMutexCreate (&instance_mutex);
}

bool RtcMutex::waitForMutex(void)
{
	osStatus status = osMutexWait(id_mutex, osWaitForever);
	
	if (status == osOK)
	{
		return true;
	}
	else
	{
		return false;
	}
}
		
bool RtcMutex::releaseMutex(void)
{
	osStatus status = osMutexRelease(id_mutex);

	if (status == osOK)
	{
		return true;
	}
	else
	{
		return false;
	}
}