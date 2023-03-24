/*
 * uart_hd.c
 *
 *  Created on: Mar 23, 2023
 *      Author: v
 */

#include "uart_hd.h"
#include "stm32f1xx_ll_usart.h"

//UART read data buffer
#define RX_BUF_LEN 128
volatile uint8_t USART_RxBuffer[RX_BUF_LEN];
volatile int USART_RxGetPosition = 0;
volatile int USART_RxPutPosition = 0;

static USART_TypeDef* USARTx_HD; // stores configured USART Half Duplex for further communication

/*---------------
Configure USART in Device Configuration Tool as Half-Duplex, Baudrate 1000000, 8Bit, Parity None
Post Init stores selected USART interface and enables Rx interrupt
------------------*/
void USART_HD_PostInit(USART_TypeDef* USARTx)
{
	USARTx_HD = USARTx;
	LL_USART_SetTransferDirection(USARTx_HD, LL_USART_DIRECTION_RX);
	LL_USART_EnableIT_RXNE(USARTx_HD);
	LL_USART_Enable(USARTx_HD);
}

/*---------------
IRQ handler should be placed in corresponding USARTx_IRQHandler
Reads incoming byte and strores in receive buffer
------------------*/
void USART_HD_IRQHandler()
{
	LL_USART_ClearFlag_ORE(USARTx_HD);
	USART_RxBuffer[USART_RxPutPosition] = LL_USART_ReceiveData8(USARTx_HD);
	USART_RxPutPosition = (USART_RxPutPosition + 1) % RX_BUF_LEN;
}

/*---------------
Kinda clears receive buffer
------------------*/
void USART_HD_BufferFlush(void)
{
	USART_RxGetPosition = USART_RxPutPosition = 0;
}

/*---------------
Read byte from receive buffer, if no data available return -1
------------------*/
int16_t USART_HD_BufferRead(void)
{
	if (USART_RxGetPosition != USART_RxPutPosition)
	{
		uint8_t Data = USART_RxBuffer[USART_RxGetPosition];
		USART_RxGetPosition = (USART_RxGetPosition + 1) % RX_BUF_LEN;
		return Data;
	}
	else
	{
		return -1;
	}
}

/*---------------
Transmit data. Changing transfer direction is needed to prevent IRQ firing when data is transmitting
------------------*/
void USART_HD_Send(uint8_t *buf, uint8_t len)
{
	LL_USART_SetTransferDirection(USARTx_HD, LL_USART_DIRECTION_TX);
	for (int i = 0; i < len; i++)
	{
		LL_USART_TransmitData8(USARTx_HD, buf[i]);
		while(!LL_USART_IsActiveFlag_TXE(USARTx_HD));
	}
	while(!LL_USART_IsActiveFlag_TC(USARTx_HD));
	LL_USART_SetTransferDirection(USARTx_HD, LL_USART_DIRECTION_RX);
}

