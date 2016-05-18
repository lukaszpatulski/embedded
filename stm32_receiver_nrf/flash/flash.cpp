#include "flash.h"

void Flash::write(void)
{
	unlock();
	
	clearFlags();
	
	
	lock();
	
}

void Flash::unlock(void)
{
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
}

void Flash::lock(void)
{
  FLASH->CR |= FLASH_CR_LOCK;
}

void Flash::clearFlags(void)
{
	/*  End of operation; Write Protection Error. Reset by writing a 1 */
	FLASH->SR |= FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR;
	            
}

void Flash::pageErase(uint32_t Page_Address)
{
	/* Page Erase chosen */
	FLASH->CR |= FLASH_CR_PER;
	/* Flash Address */
	FLASH->AR = Page_Address;
	/* Start erasing last page of flash memory */
	FLASH->CR |= FLASH_CR_STRT;
	
	/* Wait for end of erasing page */
	while (!(FLASH->SR & FLASH_SR_EOP)) {}
	
	/* Disable the PER Bit */
  FLASH->CR &= ~FLASH_CR_PER;
}

void Flash::programHalfWordFlash(uint32_t address, uint16_t data)
{
	/* Flash programming chosen */
	FLASH->CR |= FLASH_CR_PG;
	
	*(__IO uint16_t*)address = data;
	
	/* Wait for end of programming page */
	while (!(FLASH->SR & FLASH_SR_EOP)) {}
	
	/* Disable the PG Bit */
	FLASH->CR &= ~FLASH_CR_PG;
}




