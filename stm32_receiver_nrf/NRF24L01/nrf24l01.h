#ifndef NRF24L01_H
#define NRF24L01_H 100

#include "stm32f10x.h"
#include <string.h>
#include "NRF24L01_constants.h"
#include "stdlib.h"


typedef enum {
	NRF24L01_Transmit_Status_Lost = 0,		//Message is lost, reached max retransmissions
	NRF24L01_Transmit_Status_Ok = 1,			//Message sent successfully
	NRF24L01_Transmit_Status_Sending = 0xFF	//Message is still sending
} NRF24L01_Transmit_Status_t;

typedef enum {
	NRF24L01_DataRate_2M,		// 2Mbps
	NRF24L01_DataRate_1M,		// 1Mbps
	NRF24L01_DataRate_250k		// 250kbps
} NRF24L01_DataRate_t;

typedef enum {
	NRF24L01_OutputPower_M18dBm,	// -18dBm
	NRF24L01_OutputPower_M12dBm,	// -12dBm
	NRF24L01_OutputPower_M6dBm,	// -6dBm
	NRF24L01_OutputPower_0dBm	// 0dBm
} NRF24L01_OutputPower_t;

typedef struct {
	uint8_t PayloadSize;				//Payload size
	uint8_t Channel;					//Channel selected
	NRF24L01_OutputPower_t OutPwr;	//Output power
	NRF24L01_DataRate_t DataRate;	//Data rate
} NRF24L01_t;

class NRF24L01_base 
{
	
public:

	uint8_t NRF24L01_Init(uint8_t channel, uint8_t payload_size);

	void NRF24L01_SetMyAddress(uint8_t* adr);

	void NRF24L01_SetPipe2Address(uint8_t adr);
	void NRF24L01_SetPipe3Address(uint8_t adr);
	void NRF24L01_SetPipe4Address(uint8_t adr);
	void NRF24L01_SetPipe5Address(uint8_t adr);

	void NRF24L01_SetTxAddress(uint8_t* adr);

	uint8_t NRF24L01_GetRetransmissionsCount(void);

	void NRF24L01_PowerUpTx(void);

	void NRF24L01_PowerUpRx(void);

	void NRF24L01_PowerDown(void);

	NRF24L01_Transmit_Status_t NRF24L01_GetTransmissionStatus(void);

	void NRF24L01_Transmit(uint8_t *data);

	void NRF24L01_GetData(uint8_t *data);

	uint8_t NRF24L01_DataReady(void);

	bool NRF24L01_Empty_RX_Fifo(void);

	void NRF24L01_SetChannel(uint8_t channel);

	void NRF24L01_SetRF(NRF24L01_DataRate_t DataRate, NRF24L01_OutputPower_t OutPwr);

protected:
		
	uint8_t NRF24L01_GetStatus(void);
	void NRF24L01_WriteBit(uint8_t reg, uint8_t bit, BitAction value);
	uint8_t NRF24L01_ReadBit(uint8_t reg, uint8_t bit);
	uint8_t NRF24L01_ReadRegister(uint8_t reg);
	void NRF24L01_ReadRegisterMulti(uint8_t reg, uint8_t* data, uint8_t count);
	void NRF24L01_WriteRegister(uint8_t reg, uint8_t value);
	void NRF24L01_WriteRegisterMulti(uint8_t reg, uint8_t *data, uint8_t count);
	void NRF24L01_SoftwareReset(void);
	uint8_t NRF24L01_RxFifoEmpty(void);

	virtual void NRF24L01_CE_Low(void)   = 0;
	virtual void NRF24L01_CE_High(void)  = 0;
	virtual void NRF24L01_CSN_Low(void)  = 0;
	virtual void NRF24L01_CSN_High(void) = 0;

	FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);

	void SPI_SendMulti(uint8_t *dataOut, uint8_t *dataIn, uint16_t count);

	void SPI_WriteMulti(uint8_t *dataOut, uint16_t count);

	void SPI_ReadMulti(uint8_t *dataIn, uint8_t dummy, uint16_t count);
	
	/* Virtual functions */
	
	virtual void NRF24L01_InitPins(void) = 0;
	virtual void SPI_Init(void) = 0;
	virtual uint8_t SPI_Send(uint8_t data) = 0;
	
	virtual void InitInterrupt_Pin(void) = 0;

};



class NRF24L01_manager : public NRF24L01_base 
{	

private:
	
	/* Receiver address */
	uint8_t TxAddress[5];
	/* My address */
	uint8_t MyAddress[5];

	template <class T> class Queue 
	{
	private:
		
		int startId;
		int endId;

		enum 
		{	
			QUEUE_MAX   = 10,
			DATA_LENGTH = 32
		};
		
		T receivedData[QUEUE_MAX][DATA_LENGTH];
		
		int getNextId(int currentId);
		
	public:

		Queue(void);
		
		bool pop(T * dataIn);
		bool push(T * dataIn);
		void flushQueue(void);
		
	};
	
	Queue<uint8_t> queue;
	
public:
	
  /* Constructor */
	NRF24L01_manager(void);
	
	virtual void NRF24L01_InitPins(void);
	virtual void SPI_Init(void);
	virtual uint8_t SPI_Send(uint8_t data);
	virtual void NRF24L01_CE_Low(void);
	virtual void NRF24L01_CE_High(void);
	virtual void NRF24L01_CSN_Low(void);
	virtual void NRF24L01_CSN_High(void);

	virtual void InitInterrupt_Pin(void);

	/* Call functions with configuration parameters */
	void InitializeNRF24L01(void);

	bool ReceiveData(void);
	bool getData(uint8_t * dataIn);
};

extern NRF24L01_manager NRF24L01_manager_object;


#endif

