/*
 * SCS.c
 * SCS Serial Servo Protocol Program
 * Date: 2022.3.29
 * Author: 
 */

#include <stdlib.h>
#include "INST.h"
#include "SCS.h"

static uint8_t Level = 1; //Servo return level
static uint8_t End = 1; //processor endian structure (0-big-endian, 1-little-endian)
static uint8_t Error = 0;//Servo status
uint8_t syncReadRxPacketIndex;
uint8_t syncReadRxPacketLen;
uint8_t *syncReadRxPacket;
uint8_t *syncReadRxBuff;
uint16_t syncReadRxBuffLen;
uint16_t syncReadRxBuffMax;
uint32_t syncTimeOut;

//One 16-bit number split into two 8-bit numbers
void Host2SCS(uint8_t *DataL, uint8_t *DataH, int Data)
{
	if (End)
	{
		*DataL = (Data >> 8);
		*DataH = (Data & 0xff);
	}
	else
	{
		*DataH = (Data >> 8);
		*DataL = (Data & 0xff);
	}
}

//Two 8-bit numbers combined into one 16-bit number
int SCS2Host(uint8_t DataL, uint8_t DataH)
{
	int Data;
	if (End)
	{
		Data = DataL;
		Data <<= 8;
		Data |= DataH;
	}
	else
	{
		Data = DataH;
		Data <<= 8;
		Data |= DataL;
	}
	return Data;
}

void writeBuf(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen,
		uint8_t Fun)
{
	uint8_t i;
	uint8_t msgLen = 2;
	uint8_t bBuf[6];
	uint8_t CheckSum = 0;
	bBuf[0] = 0xff;
	bBuf[1] = 0xff;
	bBuf[2] = ID;
	bBuf[4] = Fun;
	if (nDat)
	{
		msgLen += nLen + 1;
		bBuf[3] = msgLen;
		bBuf[5] = MemAddr;
		writeSCS(bBuf, 6);

	}
	else
	{
		bBuf[3] = msgLen;
		writeSCS(bBuf, 5);
	}
	CheckSum = ID + msgLen + Fun + MemAddr;
	if (nDat)
	{
		for (i = 0; i < nLen; i++)
		{
			CheckSum += nDat[i];
		}
		writeSCS(nDat, nLen);
	}
	CheckSum = ~CheckSum;
	writeSCS(&CheckSum, 1);
}

//general write command
//ID - ServoID, MemAddr - memory table address, nDat - write data, nLen - write length
int genWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen)
{
	rFlushSCS();
	writeBuf(ID, MemAddr, nDat, nLen, INST_WRITE);
	wFlushSCS();
	return Ack(ID);
}

// Write asynchronous command
//ID - ServoID, MemAddr - memory table address, nDat - write data, nLen - write length
int regWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen)
{
	rFlushSCS();
	writeBuf(ID, MemAddr, nDat, nLen, INST_REG_WRITE);
	wFlushSCS();
	return Ack(ID);
}

//Execute asynchronous command
int regAction(uint8_t ID)
{
	rFlushSCS();
	writeBuf(ID, 0, NULL, 0, INST_REG_ACTION);
	wFlushSCS();
	return Ack(ID);
}

//Write synchronous command
//ID[] - ServoID array, IDN - array length, MemAddr - memory table address, nDat - write data, nLen - write length
void syncWrite(uint8_t ID[], uint8_t IDN, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen)
{
	uint8_t mesLen = ((nLen+1)*IDN+4);
	uint8_t Sum = 0;
	uint8_t bBuf[7];
	uint8_t i, j;
	
	bBuf[0] = 0xff;
	bBuf[1] = 0xff;
	bBuf[2] = 0xfe;
	bBuf[3] = mesLen;
	bBuf[4] = INST_SYNC_WRITE;
	bBuf[5] = MemAddr;
	bBuf[6] = nLen;
	
	rFlushSCS();
	writeSCS(bBuf, 7);

	Sum = 0xfe + mesLen + INST_SYNC_WRITE + MemAddr + nLen;

	for(i=0; i<IDN; i++){
		writeSCS(&ID[i], 1);
		writeSCS(nDat+i*nLen, nLen);
		Sum += ID[i];
		for(j=0; j<nLen; j++){
			Sum += nDat[i*nLen+j];
		}
	}
	Sum = ~Sum;
	writeSCS(&Sum, 1);
	wFlushSCS();
}

int writeByte(uint8_t ID, uint8_t MemAddr, uint8_t bDat)
{
	rFlushSCS();
	writeBuf(ID, MemAddr, &bDat, 1, INST_WRITE);
	wFlushSCS();
	return Ack(ID);
}

int writeWord(uint8_t ID, uint8_t MemAddr, uint16_t wDat)
{
	uint8_t buf[2];
	Host2SCS(buf+0, buf+1, wDat);
	rFlushSCS();
	writeBuf(ID, MemAddr, buf, 2, INST_WRITE);
	wFlushSCS();
	return Ack(ID);
}

//Read command
//ID - ServoID, MemAddr - memory table address, nData - write data, nLen - write length
int Read(uint8_t ID, uint8_t MemAddr, uint8_t *nData, uint8_t nLen)
{
	int Size;
	uint8_t bBuf[4];
	uint8_t calSum;
	uint8_t i;
	rFlushSCS();
	writeBuf(ID, MemAddr, &nLen, 1, INST_READ);
	wFlushSCS();
	if(!checkHead()){
		return 0;
	}
	Error = 0;
	if(readSCS(bBuf, 3)!=3){
		return 0;
	}
	Size = readSCS(nData, nLen);
	if(Size!=nLen){
		return 0;
	}
	if(readSCS(bBuf+3, 1)!=1){
		return 0;
	}
	calSum = bBuf[0]+bBuf[1]+bBuf[2];
	for(i=0; i<Size; i++){
		calSum += nData[i];
	}
	calSum = ~calSum;
	if(calSum!=bBuf[3]){
		return 0;
	}
	Error = bBuf[2];
	return Size;
}

//Read 1 byte, timeout returns -1
int readByte(uint8_t ID, uint8_t MemAddr)
{
	uint8_t bDat;
	int Size = Read(ID, MemAddr, &bDat, 1);
	if(Size!=1){
		return -1;
	}else{
		return bDat;
	}
}

//Read 2 bytes, timeout returns -1
int readWord(uint8_t ID, uint8_t MemAddr)
{	
	uint8_t nDat[2];
	int Size;
	uint16_t wDat;
	Size = Read(ID, MemAddr, nDat, 2);
	if(Size!=2)
		return -1;
	wDat = SCS2Host(nDat[0], nDat[1]);
	return wDat;
}

//Ping command, return servo ID if found, return -1 when timeout
int Ping(uint8_t ID)
{
	uint8_t bBuf[4];
	uint8_t calSum;
	rFlushSCS();
	writeBuf(ID, 0, NULL, 0, INST_PING);
	wFlushSCS();
	Error = 0;
	if (!checkHead())
	{
		return -1;
	}

	if (readSCS(bBuf, 4) != 4)
	{
		return -1;
	}
	if (bBuf[0] != ID && ID != 0xfe)
	{
		return -1;
	}
	if (bBuf[1] != 2)
	{
		return -1;
	}
	calSum = ~(bBuf[0] + bBuf[1] + bBuf[2]);
	if (calSum != bBuf[3])
	{
		return -1;
	}
	Error = bBuf[2];
	return bBuf[0];
}

int checkHead(void)
{
	uint8_t bDat;
	uint8_t bBuf[2] = { 0, 0 };
	uint8_t Cnt = 0;
	while (1)
	{
		if (!readSCS(&bDat, 1))
		{
			return 0;
		}
		bBuf[1] = bBuf[0];
		bBuf[0] = bDat;
		if (bBuf[0] == 0xff && bBuf[1] == 0xff)
		{
			break;
		}
		Cnt++;
		if (Cnt > 10)
		{
			return 0;
		}
	}
	return 1;
}

//Command response, acknowledge
int Ack(uint8_t ID)
{
	uint8_t bBuf[4];
	uint8_t calSum;
	Error = 0;
	if (ID != 0xfe && Level)
	{
		if (!checkHead())
		{
			return 0;
		}
		if (readSCS(bBuf, 4) != 4)
		{
			return 0;
		}
		if (bBuf[0] != ID)
		{
			return 0;
		}
		if (bBuf[1] != 2)
		{
			return 0;
		}
		calSum = ~(bBuf[0] + bBuf[1] + bBuf[2]);
		if (calSum != bBuf[3])
		{
			return 0;
		}
		Error = bBuf[2];
	}
	return 1;
}

int syncReadPacketTx(uint8_t ID[], uint8_t IDN, uint8_t MemAddr, uint8_t nLen)
{
	uint8_t checkSum;
	uint8_t i;
	rFlushSCS();
	syncReadRxPacketLen = nLen;
	checkSum = (4 + 0xfe) + IDN + MemAddr + nLen + INST_SYNC_READ;
	writeByteSCS(0xff);
	writeByteSCS(0xff);
	writeByteSCS(0xfe);
	writeByteSCS(IDN + 4);
	writeByteSCS(INST_SYNC_READ);
	writeByteSCS(MemAddr);
	writeByteSCS(nLen);
	for (i = 0; i < IDN; i++)
	{
		writeByteSCS(ID[i]);
		checkSum += ID[i];
	}
	checkSum = ~checkSum;
	writeByteSCS(checkSum);
	wFlushSCS();

	syncReadRxBuffLen = readSCSTimeOut(syncReadRxBuff, syncReadRxBuffMax, syncTimeOut);
	return syncReadRxBuffLen;
}

void syncReadBegin(uint8_t IDN, uint8_t rxLen, uint32_t TimeOut)
{
	syncReadRxBuffMax = IDN * (rxLen + 6);
	syncReadRxBuff = malloc(syncReadRxBuffMax);
	syncTimeOut = TimeOut;
}

void syncReadEnd(void)
{
	if (syncReadRxBuff)
	{
		free(syncReadRxBuff);
		syncReadRxBuff = NULL;
	}
}

int syncReadPacketRx(uint8_t ID, uint8_t *nDat)
{
	uint16_t syncReadRxBuffIndex = 0;
	syncReadRxPacket = nDat;
	syncReadRxPacketIndex = 0;
	while ((syncReadRxBuffIndex + 6 + syncReadRxPacketLen) <= syncReadRxBuffLen)
	{
		uint8_t bBuf[] =
		{ 0, 0, 0 };
		uint8_t calSum = 0;
		while (syncReadRxBuffIndex < syncReadRxBuffLen)
		{
			bBuf[0] = bBuf[1];
			bBuf[1] = bBuf[2];
			bBuf[2] = syncReadRxBuff[syncReadRxBuffIndex++];
			if (bBuf[0] == 0xff && bBuf[1] == 0xff && bBuf[2] != 0xff)
			{
				break;
			}
		}
		if (bBuf[2] != ID)
		{
			continue;
		}
		if (syncReadRxBuff[syncReadRxBuffIndex++] != (syncReadRxPacketLen + 2))
		{
			continue;
		}
		Error = syncReadRxBuff[syncReadRxBuffIndex++];
		calSum = ID + (syncReadRxPacketLen + 2)
				+ syncReadRxBuff[syncReadRxBuffIndex++];
		for (uint8_t i = 0; i < syncReadRxPacketLen; i++)
		{
			syncReadRxPacket[i] = syncReadRxBuff[syncReadRxBuffIndex++];
			calSum += syncReadRxPacket[i];
		}
		calSum = ~calSum;
		if (calSum != syncReadRxBuff[syncReadRxBuffIndex++])
		{
			return 0;
		}
		return syncReadRxPacketLen;
	}
	return 0;
}

int syncReadRxPacketToByte(void)
{
	if (syncReadRxPacketIndex >= syncReadRxPacketLen)
	{
		return -1;
	}
	return syncReadRxPacket[syncReadRxPacketIndex++];
}

int syncReadRxPacketToWord(uint8_t negBit)
{
	if ((syncReadRxPacketIndex + 1) >= syncReadRxPacketLen)
	{
		return -1;
	}
	int Word = SCS2Host(syncReadRxPacket[syncReadRxPacketIndex],
			syncReadRxPacket[syncReadRxPacketIndex + 1]);
	syncReadRxPacketIndex += 2;
	if (negBit)
	{
		if (Word & (1 << negBit))
		{
			Word = -(Word & ~(1 << negBit));
		}
	}
	return Word;
}
