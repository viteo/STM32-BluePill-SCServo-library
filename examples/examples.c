/*
 * examples.c
 *
 *  Created on: Mar 23, 2023
 *      Author: v
 *
 *  Functions correspond to each file in Examples folder from original SDK
 */

#include "stm32f1xx.h"
#include "SCServo.h"
#include "stm32f1xx_ll_utils.h"

//Broadcast command example
void Example_Broadcast()
{
	while(1)
	{
		WritePosEx(0xfe, 4095, 2250, 50); //servo (broadcast) with maximum speed V=2250 steps/s, acceleration A=50 (50*100 steps/s^2), run to P1=4095
		LL_mDelay(2270); //[(P1-P0)/V]*1000+[V/(A*100)]*1000

		WritePosEx(0xfe, 0, 2250, 50); //servo (broadcast) with maximum speed V=2250 steps/s, acceleration A=50 (50*100 steps/s^2), run to P1=0
		LL_mDelay(2270);
	}
}

//Median calibration example
void Example_CalibrationOfs()
{
	CalibrationOfs(1);
	while (1);
}

/*
 * Read the servo feedback parameters: position, speed, load, voltage, temperature, movement status, current;
 * FeedBack function read back all servo parameters to the buffer, Readxxx(-1) function return the corresponding servo state from the buffer;
 * function Readxxx(ID), ID>=0, call the read command to query directly from the specified ID servo, no need to call FeedBack function.
 */
void Example_FeedBack()
{
	int Pos;
	int Speed;
	int Load;
	int Voltage;
	int Temper;
	int Move;
	int Current;
	if (FeedBack(1) != -1)
	{
		Pos = ReadPos(-1);
		Speed = ReadSpeed(-1);
		Load = ReadLoad(-1);
		Voltage = ReadVoltage(-1);
		Temper = ReadTemper(-1);
		Move = ReadMove(-1);
		Current = ReadCurrent(-1);
		//printf("Pos:%d\n", Pos);
		//printf("Speed:%d\n", Speed);
		//printf("Load:%d\n", Load);
		//printf("Voltage:%d\n", Voltage);
		//printf("Temper:%d\n", Temper);
		//printf("Move:%d\n", Move);
		//printf("Current:%d\n", Current);
		LL_mDelay(10);
	}
	else
	{
		//printf("FeedBack err\n");
		LL_mDelay(2000);
	}
	Pos = ReadPos(1);
	if (Pos != -1)
	{
		//printf("Servo position:%d\n", Pos);
		LL_mDelay(10);
	}
	else
	{
		//printf("read position err\n");
		LL_mDelay(500);
	}

	Voltage = ReadVoltage(1);
	if (Voltage != -1)
	{
		//printf("Servo Voltage:%d\n", Voltage);
		LL_mDelay(10);
	}
	else
	{
		//printf("read Voltage err\n");
		LL_mDelay(500);
	}

	Temper = ReadTemper(1);
	if (Temper != -1)
	{
		//printf("Servo temperature:%d\n", Temper);
		LL_mDelay(10);
	}
	else
	{
		//printf("read temperature err\n");
		LL_mDelay(500);
	}

	Speed = ReadSpeed(1);
	if (Speed != -1)
	{
		//printf("Servo Speed:%d\n", Speed);
		LL_mDelay(10);
	}
	else
	{
		//printf("read Speed err\n");
		LL_mDelay(500);
	}

	Load = ReadLoad(1);
	if (Load != -1)
	{
		//printf("Servo Load:%d\n", Load);
		LL_mDelay(10);
	}
	else
	{
		//printf("read Load err\n");
		LL_mDelay(500);
	}

	Current = ReadCurrent(1);
	if (Current != -1)
	{
		//printf("Servo Current:%d\n", Current);
		LL_mDelay(10);
	}
	else
	{
		//printf("read Current err\n");
		LL_mDelay(500);
	}

	Move = ReadMove(1);
	if (Move != -1)
	{
		//printf("Servo Move:%d\n", Move);
		LL_mDelay(10);
	}
	else
	{
		//printf("read Move err\n");
		LL_mDelay(500);
	}
	//printf("\n");
}

//Ping command is used to check the readiness of the corresponding servo IDs on the bus
//Broadcast Ping command is only applicable if there is only one servo on the bus
void Example_Ping()
{
	int ID = Ping(1);
	if (ID != -1)
	{
		//printf("Servo ID:%d\n", ID);
		LL_mDelay(100);
	}
	else
	{
		//printf("Ping servo ID error!\n");
		LL_mDelay(2000);
	}
}

//Servo parameter programming
void Example_Eprom()
{
	unLockEprom(1);		//Unlock EEPROM
	writeByte(1, SMS_STS_ID, 2); //Change ID from 1 to 2
	LockEprom(2);		//Lock EEPROM
}

//Asynchronous writing example
void Example_RegWritePos()
{
	while(1)
	{
		RegWritePosEx(1, 4095, 2250, 50); //Servo (ID1), with maximum speed V=2250 steps/s, acceleration A=50 (50*100 steps/s^2), running to P1=4095
		RegWritePosEx(2, 4095, 2250, 50); //Servo (ID2), with maximum speed V=2250 steps/s, acceleration A=50 (50*100 steps/s^2), running to P1=4095
		RegWriteAction(); //Execute commands
		LL_mDelay(2270); //[(P1-P0)/V]*1000+[V/(A*100)]*1000

		RegWritePosEx(1, 0, 2250, 50); //Servo (ID1), with maximum speed V=2250 steps/s, acceleration A=50 (50*100 steps/s^2), running to P1=0
		RegWritePosEx(2, 0, 2250, 50); //Servo (ID2), with maximum speed V=2250 steps/s, acceleration A=50 (50*100 steps/s^2), running to P1=0
		RegWriteAction(); //Execute commands
		LL_mDelay(2270); //[(P1-P0)/V]*1000+[V/(A*100)]*1000
	}
}

//Synchronous reading supports both SMS/STS servo models
void Example_SyncRead()
{
	int16_t Position;
	int16_t Speed;
	uint8_t ID[] = {1, 2};
	uint8_t rxPacket[4];

	syncReadBegin(sizeof(ID), sizeof(rxPacket), 1800 * 10); //10ms
	LL_mDelay(1000);

	while (1)
	{
		uint8_t i;

		syncReadPacketTx(ID, sizeof(ID), SMS_STS_PRESENT_POSITION_L, sizeof(rxPacket)); //Synchronous read instruction packet transmission
		for (i = 0; i < sizeof(ID); i++)
		{
			//Receive ID[i] synchronous read return packet
			if (!syncReadPacketRx(ID[i], rxPacket))
			{
				//printf("ID:%d sync read error!\n", ID[i]);
				continue;//Receive decoding failure
			}
			Position = syncReadRxPacketToWord(15); //Decode two bytes bit15 is the direction bit, parameter=0 means no direction bit
			Speed = syncReadRxPacketToWord(15); //Decode two bytes bit15 is the direction bit, parameter=0 means no direction bit
			//printf("ID:%d Position:%d Speed:%d ", ID[i], Position, Speed);
			LL_mDelay(10);
		}
		//printf("\n");
	}

	syncReadEnd();
}

// Simultaneous writing example
void Example_SyncWritePos()
{
	uint8_t ID[2];
	int16_t Position[2];
	uint16_t Speed[2];
	uint8_t ACC[2];
	ID[0] = 1;
	ID[1] = 2;

	Speed[0] = 1500;
	Speed[1] = 1500;
	ACC[0] = 50;
	ACC[1] = 50;

	while(1)
	{
		Position[0] = 4095;
		Position[1] = 4095;
		SyncWritePosEx(ID, 2, Position, Speed, ACC); //Servo (ID1/ID2) with maximum speed V=1500 steps/sec, acceleration A=50 (50*100 steps/sec^2), run to P1=4095
		LL_mDelay(2270); //((P1-P0)/V)*1000+(V/(A*100))*1000

		Position[0] = 0;
		Position[1] = 0;
		SyncWritePosEx(ID, 2, Position, Speed, ACC); //Servo (ID1/ID2) with maximum speed V=1500 steps/sec, acceleration A=50 (50*100 steps/sec^2), run to P1=0
		LL_mDelay(2270); //((P1-P0)/V)*1000+(V/(A*100))*1000
	}
}

//Common writing example
void Example_WritePos()
{
	while (1)
	{
		WritePosEx(1, 4095, 2250, 50); //Servo (ID1), at the highest speed V=2250 steps/s, acceleration A=50 (50*100 steps/s^2), run to P1=4095
		LL_mDelay(2270); //[(P1-P0)/V]*1000+[V/(A*100)]*1000

		WritePosEx(1, 0, 2250, 50); //Servo (ID1), at the highest speed V=2250 steps/s, acceleration A=50 (50*100 steps/s^2), run to P1=0
		LL_mDelay(2270); //[(P1-P0)/V]*1000+[V/(A*100)]*1000
	}
}

// Example of constant speed mode
void Example_WriteSpe()
{
	WheelMode(1); //constant speed mode
	LL_mDelay(1000);

	while (1)
	{
		WriteSpe(1, 2000, 50); //Servo (ID1) rotates at a maximum speed V = 2000 steps/sec and an acceleration A = 50 (50*100 steps/sec^2)
		LL_mDelay(4000);
		WriteSpe(1, 0, 50); //Servo (ID1) with acceleration A=50 (50*50 stepss/sec^2), stop rotation (V=0)
		LL_mDelay(2000);
		WriteSpe(1, -2000, 50); //Servo (ID1) rotating in reverse with maximum speed V = -2000 steps/sec and acceleration A = 50 (50*100 steps/sec^2)
		LL_mDelay(4000);
		WriteSpe(1, 0, 50); //Servo (ID1) with acceleration A=50 (50*100 steps/sec^2), stop rotation (V=0)
		LL_mDelay(2000);
	}
}

