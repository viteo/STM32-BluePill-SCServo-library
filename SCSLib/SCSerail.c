/*
 * SCServo.c
 * FeeTech servo hardware interface layer program
 * Date: 2022.3.29
 * Author: 
 */

#include "stm32f1xx.h"
#include "uart_hd.h"

const uint32_t IOTimeOut = 5000;//Communication timeout
uint8_t wBuf[128];
uint8_t wLen = 0;

int readSCSTimeOut(unsigned char *nDat, int nLen, uint32_t TimeOut)
{
	int Size = 0;
	int ComData;
	uint32_t t_user = 0;
	while (1)
	{
		ComData = USART_HD_BufferRead();
		if (ComData != -1)
		{
			if (nDat)
			{
				nDat[Size] = ComData;
			}
			Size++;
		}
		if (Size >= nLen)
		{
			break;
		}
		t_user++;
		if (t_user > TimeOut)
		{
			break;
		}
	}
	return Size;
}

//Receive Data from UART buffer
int readSCS(unsigned char *nDat, int nLen)
{
	int Size = 0;
	int ComData;
	uint32_t t_user = 0;
	while (1)
	{
		ComData = USART_HD_BufferRead();
		if (ComData != -1)
		{
			if (nDat)
			{
				nDat[Size] = ComData;
			}
			Size++;
			t_user = 0;
		}
		if (Size >= nLen)
		{
			break;
		}
		t_user++;
		if (t_user > IOTimeOut)
		{
			break;
		}
	}
	return Size;
}

//Write data to transmit buffer
int writeSCS(unsigned char *nDat, int nLen)
{
	while(nLen--){
		if(wLen<sizeof(wBuf)){
			wBuf[wLen] = *nDat;
			wLen++;
			nDat++;
		}
	}
	return wLen;
}

int writeByteSCS(unsigned char bDat)
{
	if(wLen<sizeof(wBuf)){
		wBuf[wLen] = bDat;
		wLen++;
	}
	return wLen;
}

//Bus switching delay
void SCSDelay(void)
{
	uint8_t i = IOTimeOut;
	while (i--);
}

//Receive buffer flush (discard all Rx data)
void rFlushSCS()
{
//	SCSDelay();
	USART_HD_BufferFlush();
}

//Transmit buffer flush (transmit all Tx data)
void wFlushSCS()
{
	if (wLen)
	{
		USART_HD_Send(wBuf, wLen);
		wLen = 0;
	}
}

