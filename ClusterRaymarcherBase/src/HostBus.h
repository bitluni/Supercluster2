#pragma once
#include "bus.h"
#include "timer.h"
#include <stdint.h>

class HostBus: public Bus
{
	public:
	HostBus()
		:Bus()
	{
	}

	enum ErrorCode
	{
		ERROR_SUCCESS = 0,
		ERROR_TIME_OUT_CMD_ACK = 1,
		ERROR_TIME_OUT_CMD_NACK = 2,
		ERROR_TIME_OUT_CMD_FULL_NACK = 3,
		ERROR_TIME_OUT_CMD_FULL = 4,
		ERROR_TIME_OUT_CLK_ACK = 5,
		ERROR_TIME_OUT_CLK_NACK = 6,
		ERROR_TIME_OUT_CLK_FULL_NACK = 7,
		ERROR_TIME_OUT_CLK_FULL = 8,
		ERROR_TIME_OUT_SET_DATA = 9,
		ERROR_TIME_OUT_RESET_DATA = 10,
		ERROR_TIME_OUT_SET_EOT = 11,
		ERROR_TIME_OUT_RESET_EOT = 12,
		ERROR_TIME_OUT_SET_TYPE = 13,
		ERROR_TIME_OUT_RESET_TYPE = 14,
		ERROR_TIME_OUT_SET_CMD = 15,
		ERROR_TIME_OUT_RESET_CMD = 16,
		ERROR_TIME_OUT_SET_CLK = 17,
		ERROR_TIME_OUT_RESET_CLK = 18,
	};

	ErrorCode sendBroadcast(uint16_t lines, const uint8_t *data, int size, int signalDelayMicros = 1000)
	{
		setType(REQUEST_BROADCAST);
		setData(0xff);
		if(!waitACK(false, lines)) return ERROR_TIME_OUT_CLK_NACK;
		if(!waitCMD(0)) return ERROR_TIME_OUT_RESET_CMD;
		//if(!waitTYPE(REQUEST_BROADCAST, lines, timeout)) return ERROR_TIME_OUT_SET_TYPE;
		if(!waitDATA(0xff, lines)) return ERROR_TIME_OUT_SET_DATA;		
		setCLK();
		setCMD(lines);
		if(!waitCLK(true, lines)) return ERROR_TIME_OUT_SET_CLK;
		if(!waitCMD(lines)) return ERROR_TIME_OUT_SET_CMD;

		delayUs(signalDelayMicros);

		resetCMD(lines);
		resetCLK();
		if(!waitCMD(0)) return ERROR_TIME_OUT_RESET_CMD;
		if(!waitCLK(false, lines)) return ERROR_TIME_OUT_RESET_CLK;

		delayUs(signalDelayMicros);

		for(int i = 0; i < size; i++)
		{
			setData(data[i]);
			if(i == size - 1) 
				setEOT();
			if(!waitDATA(data[i], lines)) return ERROR_TIME_OUT_SET_DATA;
			if(i == size - 1) 
				if(!waitEOT(true, lines)) return ERROR_TIME_OUT_SET_EOT;
			setCLK();
			if(!waitCLK(true, lines)) return ERROR_TIME_OUT_SET_CLK;
			delayUs(signalDelayMicros);
			resetCLK();
			if(!waitCLK(false, lines)) return ERROR_TIME_OUT_RESET_CLK;
			delayUs(signalDelayMicros);
		}
		resetType();
		resetData();
		resetEOT();
		//if(!waitTYPE(REQUEST_NONE, lines, timeout)) return ERROR_TIME_OUT_RESET_TYPE;
		if(!waitDATA(0xff, lines)) return ERROR_TIME_OUT_RESET_DATA;		
		if(!waitEOT(false, lines)) return ERROR_TIME_OUT_RESET_EOT;
		return ERROR_SUCCESS;
	}

	//send a packet. return true is successful, false if failed
	ErrorCode sendPacket(uint16_t lines, uint8_t id, const uint8_t *data, int size)
	{
		setType(REQUEST_RECEIVE);
		setData(id);
		if(!waitACK(false, lines)) return ERROR_TIME_OUT_CLK_NACK;
		//if(!waitTYPE(REQUEST_RECEIVE, lines, timeout)) return ERROR_TIME_OUT_SET_TYPE;
		if(!waitDATA(id, lines)) return ERROR_TIME_OUT_SET_DATA;
		setCLK();
		if(!waitCLK(true, lines)) return ERROR_TIME_OUT_SET_CLK;
		
		//delayTicks(600);	//slow pullup, wait for data to be set
		setCMD(lines);
		//if there is ACK response there is no need to check CMD
		if(!waitCMD(lines)) return ERROR_TIME_OUT_SET_CMD;	
		if(!waitACK(true, lines)) return ERROR_TIME_OUT_CMD_ACK;

		resetCMD(lines);
		if(!waitCMD(0)) return ERROR_TIME_OUT_RESET_CMD;
		resetType();
		//if(!waitTYPE(REQUEST_NONE, lines, timeout)) return ERROR_TIME_OUT_RESET_TYPE;
		if(getFULL())	
		{
			setEOT(); //set end of transmission
			resetData();
			if(!waitEOT(true, lines)) return ERROR_TIME_OUT_SET_EOT;
			if(!waitDATA(0xff, lines)) return ERROR_TIME_OUT_RESET_DATA;	

			resetCLK();
			if(!waitCLK(false, lines)) return ERROR_TIME_OUT_RESET_CLK;
			if(!waitACK(false, lines)) return ERROR_TIME_OUT_CMD_FULL_NACK;

			resetEOT();
			if(!waitEOT(false, lines)) return ERROR_TIME_OUT_RESET_EOT;
			return ERROR_TIME_OUT_CMD_FULL;
		}

		resetCLK();
		//not needed with ACK
		if(!waitCLK(false, lines)) return ERROR_TIME_OUT_RESET_CLK;
		if(!waitACK(false, lines)) return ERROR_TIME_OUT_CMD_NACK;
		
		//state = STATE_TRANSMIT;
		for(int i = 0; i < size; i++)
		{
			setData(data[i]);
			if(i == size - 1)
				setEOT();
			if(!waitDATA(data[i], lines))return ERROR_TIME_OUT_SET_DATA;
			if(i == size - 1)
				if(!waitEOT(true, lines)) return ERROR_TIME_OUT_SET_EOT;
			
			//delayTicks(600);	//slow pullup, wait for data to be set
			setCLK();
			if(!waitCLK(true, lines)) return ERROR_TIME_OUT_SET_CLK;
			if(!waitACK(true, lines)) return ERROR_TIME_OUT_CLK_ACK;

			//delayTicks(128);	//slow pullup, wait for data to be set
			if(getFULL() && i < size - 1)
			{
				setEOT(); //set end of transmission
				resetData();
				if(!waitEOT(true, lines))return ERROR_TIME_OUT_SET_EOT;
				if(!waitDATA(0xff, lines)) return ERROR_TIME_OUT_RESET_DATA;	
	
				//delayTicks(600);	//slow pullup, wait for data to be set
				resetCLK();
				if(!waitCLK(false, lines)) return ERROR_TIME_OUT_RESET_CLK;
				if(!waitACK(false, lines)) return ERROR_TIME_OUT_CLK_FULL_NACK;
	
				resetEOT();
				if(!waitEOT(false, lines)) return ERROR_TIME_OUT_RESET_EOT;
					return ERROR_TIME_OUT_CLK_FULL;
			}
			resetCLK();
			if(!waitCLK(false, lines)) return ERROR_TIME_OUT_RESET_CLK;			
			if(!waitACK(false, lines)) return ERROR_TIME_OUT_CLK_NACK;
		}
		resetData();
		resetEOT();
		if(!waitDATA(0xff, lines)) return ERROR_TIME_OUT_RESET_DATA;	
		if(!waitEOT(false, lines)) return ERROR_TIME_OUT_RESET_EOT;
		return ERROR_SUCCESS;
	}

	void sendReset(uint16_t lines, uint8_t id)
	{
		uint8_t data[] = {BUS_CLIENT_RESET};
		sendPacket(lines, id, data, 1);
	}

	ErrorCode sendReset(uint16_t lines)
	{
		setType(REQUEST_RESET);		
		setData(0xff);
		//if(!waitTYPE(REQUEST_RESET, lines, timeout)) return ERROR_TIME_OUT_SET_TYPE;
		setCLK();
		if(!waitCLK(true, lines)) return ERROR_TIME_OUT_SET_CLK;
		setCMD(lines);
		if(!waitCMD(lines)) return ERROR_TIME_OUT_SET_CMD;		
		delayMs(1);

		resetCMD(lines);
		if(!waitCMD(0)) return ERROR_TIME_OUT_RESET_CMD;
		resetCLK();
		if(!waitCLK(false, lines)) return ERROR_TIME_OUT_RESET_CLK;
		resetType();
		if(!waitDATA(0xff, lines)) return ERROR_TIME_OUT_RESET_DATA;
		//if(!waitTYPE(REQUEST_NONE, lines, timeout)) return ERROR_TIME_OUT_RESET_TYPE;
		return ERROR_SUCCESS;
	}

	ErrorCode receivePacket(uint16_t lines, uint8_t id, uint8_t *data, int &size, int maxSize)
	{
		size = 0;
		setType(REQUEST_TRANSMIT);
		setData(id);
		if(!waitACK(false, lines)) return ERROR_TIME_OUT_CLK_NACK;		
		//if(!waitTYPE(REQUEST_TRANSMIT, lines, timeout)) return ERROR_TIME_OUT_SET_TYPE;
		if(!waitDATA(id, lines))return ERROR_TIME_OUT_SET_DATA;
		setCLK();
		if(!waitCLK(true, lines)) return ERROR_TIME_OUT_SET_CLK;
 
		//delayTicks(600);	//slow pullup, wait for data to be set
		setCMD(lines);
		if(!waitCMD(lines)) return ERROR_TIME_OUT_SET_CMD;		
		if(!waitACK(true, lines)) return ERROR_TIME_OUT_CMD_ACK;
		//client got packet start
		resetCMD(lines);
		if(!waitCMD(0)) return ERROR_TIME_OUT_RESET_CMD;
		//make sure no one esle is triggered
		resetType();
		resetData();
		//if(!waitTYPE(REQUEST_NONE, lines, timeout)) return ERROR_TIME_OUT_RESET_TYPE;
		if(!waitDATA(0xff, lines)) return ERROR_TIME_OUT_RESET_DATA;

		bool eot = getEOT();

		resetCLK();
		if(!waitCLK(false, lines)) return ERROR_TIME_OUT_RESET_CLK;
		if(!waitACK(false, lines)) return ERROR_TIME_OUT_CMD_NACK;

		if(eot) return ERROR_SUCCESS; //no data to receive

		while(size < maxSize)
		{
			setCLK();
			if(!waitCLK(true, lines)) return ERROR_TIME_OUT_SET_CLK;
			if(!waitACK(true, lines)) return ERROR_TIME_OUT_CLK_ACK; //timeout
			delayTicks(600);	//slow pullup, wait for data to be set
			data[size++] = getData();
			if(size == maxSize)
			{
				setEOT();
				if(!waitEOT(true, lines))return ERROR_TIME_OUT_SET_EOT;	
			}
			bool eot = getEOT();
			resetCLK();
			if(!waitCLK(false, lines)) return ERROR_TIME_OUT_RESET_CLK;
			if(!waitACK(false, lines)) return ERROR_TIME_OUT_CLK_NACK; //timeout
			if(eot) break;
		}
		resetEOT();
		if(!waitEOT(false, lines)) return ERROR_TIME_OUT_RESET_EOT;
		return ERROR_SUCCESS;
	}
};