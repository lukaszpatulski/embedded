#ifndef OS_QUEUE
#define OS_QUEUE

#include "cmsis_os.h"

class Queue
{
	public:
		
		typedef struct {                                 // Message object structure
		uint8_t    voltage;                              // AD result of measured voltage
		uint8_t    current;                              // AD result of measured current
		uint8_t    counter;                              // A counter value
		uint8_t    expander;
	  } T_MEAS;
		
		enum { QUEUE_SIZE = 10 };
	
		/* Memory pool */
		static osPoolId     id_memoryPool;
	  osPoolDef_t         os_pool_def;
		
		static uint32_t poolStorage[3+((sizeof(T_MEAS)+3)/4)*(QUEUE_SIZE)];
	
		/* Message box */
		static osMessageQId id_msgBox;
		osMessageQDef_t     os_msg_def;

	  static uint32_t messageStorage[4+(QUEUE_SIZE)];
		
		Queue(void);
		
		operator bool() const { return ((id_memoryPool != NULL) && (id_msgBox != NULL)); }
		bool operator!() const {return ((id_memoryPool == NULL) || (id_msgBox == NULL)); }
		
		bool send(uint8_t, uint8_t, uint8_t, uint8_t);
		bool receive(uint8_t *a);
};

#endif
