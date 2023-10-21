#ifndef __ARQSERIAL_H__
#define __ARQSERIAL_H__
//#define TESTFAIL

#include "RingBuffer.h"
#include "../winLibs/SerialPort.hpp"
#include <string>
#include <chrono>

const uint8_t crc_table_crc8[256] = { 0,213,127,170,254,43,129,84,41,252,86,131,215,2,168,125,82,135,45,248,172,121,211,6,123,174,4,209,133,80,250,47,164,113,219,14,90,143,37,240,141,88,242,39,115,166,12,217,246,35,137,92,8,221,119,162,223,10,160,117,33,244,94,139,157,72,226,55,99,182,28,201,180,97,203,30,74,159,53,224,207,26,176,101,49,228,78,155,230,51,153,76,24,205,103,178,57,236,70,147,199,18,184,109,16,197,111,186,238,59,145,68,107,190,20,193,149,64,234,63,66,151,61,232,188,105,195,22,239,58,144,69,17,196,110,187,198,19,185,108,56,237,71,146,189,104,194,23,67,150,60,233,148,65,235,62,106,191,21,192,75,158,52,225,181,96,202,31,98,183,29,200,156,73,227,54,25,204,102,179,231,50,152,77,48,229,79,154,206,27,177,100,114,167,13,216,140,89,243,38,91,142,36,241,165,112,218,15,32,245,95,138,222,11,161,116,9,220,118,163,247,34,136,93,214,3,169,124,40,253,87,130,255,42,128,85,1,212,126,171,132,81,251,46,122,175,5,208,173,120,210,7,83,134,44,249 };
uint8_t updateCrc(uint8_t currentCrc, int value) {
	uint8_t* addr = (uint8_t*)&crc_table_crc8[currentCrc ^ value];
	return *addr;
}

typedef void(*IdleFunction) (bool);

using namespace std;

class ARQSerial
{
private:
	SerialPort* Serial;
	uint8_t partialdatabuffer[24];
	int Arq_LastValidPacket = 255;
	IdleFunction idleFunction = 0;

#ifdef TESTFAIL
	int testfailidx = 0;
	int testfailidx2 = 0;
#endif

	int Arq_TimedRead()
	{
		int c;
		auto fsr_startMillis = chrono::high_resolution_clock::now();
		do {
			if (idleFunction != 0) idleFunction(true);
			c = Serial->read();
			if (c >= 0) {
#ifdef TESTFAIL
				testfailidx = (testfailidx + 1) % 5000;
				if (testfailidx == 500)
					return random(255);

				if (testfailidx == 1000)
					return -1;
#endif
				return c;
			}
		} while (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - fsr_startMillis).count() < 100);
		return -1;
	}

	void ProcessIncomingData() {
		int packetID, length, header, res, i, crc, nextpacketid;
		uint8_t currentCrc;

		while (Serial->available() > 0) {
			if(DataBuffer.size() > 0) 
				return;

			header = Arq_TimedRead();
			//DebugPrintLn("hello1");
			currentCrc = 0;

			if (header == 0x01) {
				uint8_t reason = 0x00;

				header = Arq_TimedRead();
				if (header != 0x01) {
					return;
				}

				if (reason == 0) {
					packetID = Arq_TimedRead();
					if (packetID < 0) {
						reason = 0x01;
					}
				}

				if (reason == 0) {
					length = Arq_TimedRead();
					if (length <= 0 || length > 32) {
						reason = 0x02;
					}
				}

				if (reason == 0)
				{	

					Serial->readSerialPort((const char*)partialdatabuffer , length);
					//std::cout << "LENGTH: " << length << " BUFFER: " << std::endl;
					for (i = 0; i < length && !reason; i++) {
						//res = Arq_TimedRead();
						//partialdatabuffer[i] = res;
						//if (res < 0) reason = 0x05;
						//std::cout << std::hex << partialdatabuffer[i] << " ";
					}
					//std::cout << std::dec << std::endl;
				}

				if (reason == 0) {
					crc = Arq_TimedRead();
					if (crc < 0) {
						reason = 0x03;
					}
				}

				if (reason == 0) {
					currentCrc = updateCrc(currentCrc, packetID);
					currentCrc = updateCrc(currentCrc, length);
					for (i = 0; i < length; i++) {
						currentCrc = updateCrc(currentCrc, partialdatabuffer[i]);
					}

					if (crc != currentCrc) {
						reason = 0x04;
					}
				}

				if (reason == 0) {
					nextpacketid = Arq_LastValidPacket > 127 ? 0 : Arq_LastValidPacket + 1;

					//cout << "DataBuffer: " << endl;
					if (packetID == nextpacketid || packetID == 255) {
						for (i = 0; i < length; i++) {
							DataBuffer.push(partialdatabuffer[i]);
							//cout << DataBuffer[i] << " ";
						}
						//cout << endl;
						Arq_LastValidPacket = packetID;
					}
#ifdef TESTFAIL
					testfailidx = (testfailidx + 1) % 5000;
					if (testfailidx != 788) {
						SendAcq(packetID);
					}
#else
					SendAcq(packetID);
#endif
				}

				if (reason > 0) {
					SendNAcq(Arq_LastValidPacket, reason);
				}
			}
		}
	}

	void SendAcq(uint8_t packetId)
	{
		Serial->write(0x03);
		Serial->write(packetId);
		Serial->flush();
	}

	void SendNAcq(uint8_t lastKnownValidPacket, uint8_t reason)
	{
		Serial->write(0x04);
		Serial->write(lastKnownValidPacket);
		Serial->write(reason);
		Serial->flush();
	}

public:

	ARQSerial(SerialPort* x) {
		this->Serial = x;
	}
	
	RingBuffer<uint8_t, 96> DataBuffer;

	void setIdleFunction(IdleFunction function) {
		idleFunction = function;
	}

	void CustomPacketStart(uint8_t packetType, uint8_t length) {
		Serial->write(0x09);
		Serial->write(packetType);
		Serial->write(length);
	}

	void CustomPacketSendByte(uint8_t data) {
		Serial->write(data);
	}

	void CustomPacketEnd() {
		//Serial->write(0x00);
	}

	int read() {
		auto fsr_startMillis = chrono::high_resolution_clock::now();
		do {
			if (idleFunction != 0) idleFunction(false);

			if (DataBuffer.size() > 0) {
				//cout << (int)DataBuffer.size() << endl;
				uint8_t res = 0;
				DataBuffer.pop(res);
				return (int)res;
			}
			ProcessIncomingData();
		} while (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - fsr_startMillis).count() < 400 || DataBuffer.size() > 0);
		//DebugPrintLn("Read timeout !");
		//cout << (int)DataBuffer.size() << " " << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - fsr_startMillis).count() << endl;
		return -1;
	}

	int Available() {
		if (idleFunction != 0) idleFunction(false);
		if (DataBuffer.size() == 0) {
			ProcessIncomingData();
		}
		return DataBuffer.size();
	}

	void Write(uint8_t data) {
		Serial->write(0x08);
		Serial->write(data);
		Serial->flush();
	}

	void Print(char data)
	{
		Write((uint8_t)data);
	}

	void Print(const char str[]) {
		int len = strlen(str);
		Serial->write(0x06);
		Serial->write(len);
		Serial->write(str);
		Serial->write(0x20);
		Serial->flush();
	}

	void WriteString(string& data)
	{
		int len = data.length();
		Serial->write(0x06);
		Serial->write(len);
		for (int i = 0; i < len; i++)
			Serial->write(data[i]);
		Serial->write(0x20);
		Serial->flush();
	}

	void PrintString(const char str[]) {
		int len = strlen(str);
		Serial->write(0x06);
		Serial->write(len);
		Serial->write(str);
		Serial->write(0x20);
		Serial->flush();
	}

	void PrintLn(const char str[]) {
		int len = strlen(str);
		Serial->write(0x06);
		Serial->write(len + 1);
		Serial->write(str);
		Serial->write('\n');
		Serial->write(0x20);
		Serial->flush();
	}

	void PrintLn(string& data)
	{
		Serial->write(0x06);
		Serial->write(data.length() + 1);
		Serial->print(data);
		Serial->print('\n');
		Serial->write(0x20);
		Serial->flush();
	}

	void PrintLn() {
		Write('\n');
	}

	string ReadStringUntil(char terminator1, char terminator2) {
		string ret;
		int c = read();
		while (c >= 0 && c != terminator1 && c != terminator2)
		{
			ret += (char)c;
			c = read();
		}
		return ret;
	}

	void ReadStringUntil(char buffer[], int size, char terminator) {
		int pos = 0;

		int c = read();
		while (c >= 0 && c != terminator && pos < size-1)
		{
			buffer[pos] = (char)c;
			c = read();
			pos++;
		}
		buffer[pos] = 0;
		
	}

	string ReadStringUntil(char terminator1) {
		string ret;
		int c = read();
		while (c >= 0 && c != terminator1)
		{
			ret += (char)c;
			c = read();
		}
		return ret;
	}

	void DebugPrintLn(string& data)
	{
		Serial->write(0x07);
		Serial->write(data.length() + 1);
		Serial->print(data);
		Serial->print('\n');
		Serial->write(0x20);
		Serial->flush();
	}

	void DebugPrint(char data)
	{
		Serial->write(0x07);
		Serial->write(1);
		Serial->print(data);
		Serial->write(0x20);
		Serial->flush();
	}

	void DebugPrintLn(const char str[]) {
		Serial->write(0x07);
		Serial->write((uint8_t)(strlen(str) + 1));
		Serial->print(str);
		Serial->print('\n');
		Serial->write(0x20);
		Serial->flush();
	}

	void flush() {
		Serial->flush();
	}
};

#endif
