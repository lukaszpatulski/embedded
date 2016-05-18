#ifndef __FLASH
#define __FLASH 22

#define FLASH_KEY1               ((uint32_t)0x45670123)
#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)

#include <stm32f10x.h>

class Flash
{
	
public:

	void write(void);
	void pageErase(uint32_t Page_Address);
	void programHalfWordFlash(uint32_t address, uint16_t data);

private:

	void unlock(void);

	void lock(void);

	void clearFlags(void);

};

#endif
