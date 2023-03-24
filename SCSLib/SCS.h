/*
 * SCS.h
 * FeeTech serial servo communication protocol program
 * Date: 2022.3.29
 * Author:
 */

#ifndef _SCS_H
#define _SCS_H

#include <stdint.h>

extern int genWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen); //General write command
extern int regWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen); //Write asynchronous command
extern int regAction(uint8_t ID); //Execute asynchronous command
extern void syncWrite(uint8_t ID[], uint8_t IDN, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen); //Write synchronous command
extern int writeByte(uint8_t ID, uint8_t MemAddr, uint8_t bDat); //Write 1 byte
extern int writeWord(uint8_t ID, uint8_t MemAddr, uint16_t wDat); //Write 2 bytes
extern int Read(uint8_t ID, uint8_t MemAddr, uint8_t *nData, uint8_t nLen); //Read command
extern int readByte(uint8_t ID, uint8_t MemAddr); //Read 1 byte
extern int readWord(uint8_t ID, uint8_t MemAddr); //Read 2 bytes
extern int Ping(uint8_t ID); //Ping command
extern int syncReadPacketTx(uint8_t ID[], uint8_t IDN, uint8_t MemAddr, uint8_t nLen); //Synchronous read instruction packet transmission
extern int syncReadPacketRx(uint8_t ID, uint8_t *nDat); //Synchronous read return packet decoding, success returns the number of memory bytes, failure returns 0
extern int syncReadRxPacketToByte(void); //Decode a byte
extern int syncReadRxPacketToWord(uint8_t negBit); //Decode two bytes, negBit is the direction, negBit=0 means no direction
extern void syncReadBegin(uint8_t IDN, uint8_t rxLen, uint32_t TimeOut); //Synchronous read start
extern void syncReadEnd(void); //Synchronous read end

extern void writeBuf(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen, uint8_t Fun);
extern void Host2SCS(uint8_t *DataL, uint8_t *DataH, int Data); //One 16-digit number split into two 8-digit numbers
extern int SCS2Host(uint8_t DataL, uint8_t DataH); //Two 8-digit numbers combined into one 16-digit number
extern int Ack(uint8_t ID); //Acknowledge, respond
extern int checkHead(void); //Frame header detection

//Hardware Interface Functions
extern int writeSCS(uint8_t *nDat, int nLen);
extern int writeByteSCS(unsigned char bDat);
extern int readSCS(uint8_t *nDat, int nLen);
extern int readSCSTimeOut(uint8_t *nDat, int nLen, uint32_t TimeOut);
extern void rFlushSCS(void);
extern void wFlushSCS(void);

#endif
