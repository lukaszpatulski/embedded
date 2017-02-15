#ifndef __FLASH
#define __FLASH 22

#define FLASH_KEY1               ((uint32_t)0x45670123)
#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)

#include <stm32f10x.h>

class Flash
{
	
public:
	
	Flash(void);

	void write(uint16_t data);
	void pageErase();
	void programHalfWordFlash(uint16_t data);
	uint16_t readFlash(void);

private:
	
	uint32_t baseAddress_127kB;

	void unlock(void);

	void lock(void);

	void clearFlags(void);

};

#endif
