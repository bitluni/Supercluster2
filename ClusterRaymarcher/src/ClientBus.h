#pragma	once
#include <stdint.h>
#include <ch32v00x.h>
#include "bus.h"

class ClientBus: public Bus
{
	public:
	bool busy;
	uint32_t lastIrqTime;

	RingBuffer<> inBuffer;
	RingBuffer<> outBuffer;

	ClientBus()
		:Bus(),
		busy(false)
	{
	}

	void handleRequest()
	{
		lastIrqTime = getTime();
		RequestType type = getType();
		if(state != STATE_IDLE || type == REQUEST_RESET)
		{
			if(state == STATE_RECEIVE || state == STATE_RECEIVE_BROADCAST) disableReceive();
			if(state == STATE_TRANSMIT) disableTransmit();
			inBuffer.clear();
			outBuffer.clear();
			state = STATE_IDLE;
			resetSignals(0);
		}
		switch(type)
		{
			case REQUEST_RECEIVE:
				{
					//debug(1);
					if(getData() != id) break;
					if(inBuffer.space() == 0) 
						setFULL();
					enableReceive();
					state = STATE_RECEIVE;
					setACK();
					return;
				}
			case REQUEST_BROADCAST:
				{
					if(getData() != 255) break;
					if(inBuffer.space() == 0) break;
					enableReceive();
					state = STATE_RECEIVE_BROADCAST;
					return;
				}
			case REQUEST_TRANSMIT:
				{
					if(getData() != id) break;
					enableTransmit();
					state = STATE_TRANSMIT;
					if(outBuffer.size == 0 || busy) setEOT();
					setACK();
					return;
				}
			case REQUEST_RESET:
				break;
		}
	}

	void handleDataClockHigh()
	{
		lastIrqTime = getTime();
		//debug(1);
		bool eot = getEOT();
		resetACK();
		if(eot)
		{
			disableReceive();
			resetFULL();
			resetEOT();
			resetData();
			//debug(0);
			state = STATE_IDLE;
		}
	}

	void handleDataClockLow()
	{
		lastIrqTime = getTime();
		switch(state)
		{
			case STATE_RECEIVE:
			{
				uint8_t data = getData();
				inBuffer.write(data);
				if(inBuffer.space() == 0)
					setFULL();
				setACK();
				return;
			}
			case STATE_TRANSMIT:
			{
				uint8_t data;
				outBuffer.read(data);
				setData(data);
				if(outBuffer.size == 0) 
					setEOT();
				waitDATA(data, 0, timeoutSignalChange);
				if(outBuffer.size == 0)
					waitEOT(true, 0, timeoutSignalChange);
				setACK();
				return;
			}
			case STATE_RECEIVE_BROADCAST:
			{
				uint8_t data = getData();
				inBuffer.write(data); //TODO: if fails remove broken packet
				return;
			}
			default:
				state = STATE_ERROR; //error, not in receive or transmit state
		}
	}

	virtual void setCMD(uint16_t lines) {};
	virtual void resetCMD(uint16_t lines) {};
	virtual void setCLK() {};
	virtual void resetCLK() {};
	virtual void resetType() {};

	bool processTimeout()
	{
		if(state == STATE_IDLE) return false;
		if(getTime() - lastIrqTime > timeoutResponse)
		{
			resetSignals(0);
			inBuffer.clear();
			outBuffer.clear();
			state = STATE_IDLE;
			return true; //timeout
		}
		return false;
	}
};
