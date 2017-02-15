#include "cmsis_os.h"

#include "os_timer.h"
#include "os_threads.h"
#include "os_queue.h"


void send_thread (void const *argument) {
  T_MEAS    *mptr;
 
  mptr = (T_MEAS*)osPoolAlloc(mpool);                     // Allocate memory for the message
  mptr->voltage = 22;                        // Set the message content
  mptr->current = 1;
  mptr->counter = 12;
	mptr->expander = 3;
  osMessagePut(MsgBox, (uint32_t)mptr, osWaitForever);  // Send Message
  osDelay(100);
 
  mptr = (T_MEAS*)osPoolAlloc(mpool);                     // Allocate memory for the message
  mptr->voltage = 3;                        // Prepare a 2nd message
  mptr->current = 1;
  mptr->counter = 3;
	mptr->expander = 3;
  osMessagePut(MsgBox, (uint32_t)mptr, osWaitForever);  // Send Message
  osThreadYield();                               // Cooperative multitasking
                                                 // We are done here, exit this thread
}

void recv_thread (void const *argument) {
  T_MEAS  *rptr;
  osEvent  evt;
   
  for (;;) {
    evt = osMessageGet(MsgBox, osWaitForever);  // wait for message
    if (evt.status == osEventMessage) {
      rptr = (T_MEAS*)evt.value.p;
      uint8_t vv = rptr->voltage;
      vv = rptr->current;
      vv = rptr->counter;
			vv = rptr->expander;

      osPoolFree(mpool, rptr);                  // free memory allocated for message
    }
  }
}

void foo(void)
{
	osSignalSet(CommunicationThread::id_thread , 0x01);
}



  /* Queues */
	Queue queue = Queue();
	
	/* Timers */
	RTX_timer os_timerLedBlinking = RTX_timer(osTimerPeriodic);
	
	/* Threads */
	CommunicationThread os_communicationThread = CommunicationThread(osPriorityNormal, 1, 12);

int main(void)
{

	if (!queue || !os_timerLedBlinking || !os_communicationThread)
	{
		/* Error */
		/* Blink sth */
	}
	
	os_timerLedBlinking.start_timer(1000);
	
	return 0;

}
