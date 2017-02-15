#ifndef OS_TIMER
#define OS_TIMER

#include "cmsis_os.h"

class RTX_timer_blinker
{
	private:
		
		osTimerDef_t instance_timer;
		static osTimerId id_timer;
	  static uint32_t os_timer_storage[6];
	
	public:
		
		static void collback_function(void const *arg);
		
		RTX_timer_blinker(os_timer_type type);
	  
		bool start_timer(int delay);
		bool stop_timer(void);

		operator bool() const { return (id_timer != NULL); }
		bool operator!() const {return (id_timer == NULL); }
};

#endif