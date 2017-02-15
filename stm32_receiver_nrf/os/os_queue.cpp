#include "os_queue.h"

osPoolId Queue::id_memoryPool = NULL;
osMessageQId Queue::id_msgBox = NULL;

uint32_t Queue::messageStorage[4+(QUEUE_SIZE)] = {0};
uint32_t Queue::poolStorage[3+((sizeof(T_MEAS)+3)/4)*(QUEUE_SIZE)] = {0};

Queue::Queue(void)
{
	os_pool_def.pool_sz = QUEUE_SIZE;
	os_pool_def.item_sz = sizeof(T_MEAS);
	os_pool_def.pool = poolStorage;
	
	id_memoryPool = osPoolCreate(&os_pool_def);                 // create memory pool
	
	os_msg_def.queue_sz = QUEUE_SIZE;
	os_msg_def.pool     = messageStorage;
	
	id_msgBox = osMessageCreate(&os_msg_def, NULL);  // create msg queue

}

bool Queue::send(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
	T_MEAS* mptr;
	
	mptr = (T_MEAS*)osPoolAlloc(id_memoryPool);                     // Allocate memory for the message
	
	if (mptr == NULL) {
		return false;
	}
	
  mptr->voltage  = a;                        // Set the message content
  mptr->current  = b;
  mptr->counter  = c;
	mptr->expander = d;
  osStatus putStatus = osMessagePut(id_msgBox, (uint32_t)mptr, osWaitForever);  // Send Message
	
	if (putStatus != osOK) {
		return false;
	}
	
	return true;

}

bool Queue::receive(uint8_t *reveivedValues)
{
	T_MEAS  *rptr;
  osEvent  evt;

	evt = osMessageGet(id_msgBox, osWaitForever);  // wait for message
  if (evt.status == osEventMessage) {
  
		rptr = (T_MEAS*)evt.value.p;
		
		reveivedValues[0] = rptr->voltage;
    reveivedValues[1] = rptr->current;
    reveivedValues[2] = rptr->counter;
		reveivedValues[3] = rptr->expander;
		
		osPoolFree(id_memoryPool, rptr);
		
		return true;
	}
	else
	{
		return false;
	}
	
}
