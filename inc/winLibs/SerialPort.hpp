/*
* Author: Manash Kumar Mandal
* Modified Library introduced in Arduino Playground which does not work
* This works perfectly
* LICENSE: MIT
*/

#pragma once

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 255

#include <iostream>
#include <vector>
//#include "stdafx.h"
#include <string>
#include <windows.h>
using namespace std;

class SerialPort
{
private:
    HANDLE handler;
    bool connected;
    COMSTAT status;
    DWORD errors;
public:
    explicit SerialPort(const char *portName, int baudrate);
    ~SerialPort();

    int readSerialPort(const char *buffer, unsigned int buf_size);
    bool writeSerialPort(const char *buffer, unsigned int buf_size);
    bool isConnected();
    void closeSerial();
    uint8_t read();
    int flush();
    bool available();
    bool write(uint8_t x);
    bool write(const char * x);
    bool print(string& x);
    bool print(const char* x);
    bool print(char x);
    bool SetBaudRate(int baud_rate);
    static std::vector<char*> getComPorts();
};

std::vector<char*> SerialPort::getComPorts() {
    std::vector<char*> v = {};
    DWORD test;
    char lpTargetPath[5000];
    char* cstr;
    for(int i=0; i<255; i++) // checking ports from COM0 to COM255
    {
        string str = "COM" + to_string(i);
        test = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

            // Test the return value and error if any
        if(test!=0) {//QueryDosDevice returns zero if it didn't find an object
            cstr = new char[str.size()+1];
            for(int i=0; i<str.size(); i++)
                cstr[i] = str[i];
            cstr[str.size()] = '\0';
            v.push_back(cstr); // add to the ComboBox
        }
        if(::GetLastError()==ERROR_INSUFFICIENT_BUFFER)
        {
            lpTargetPath[10000]; // in case the buffer got filled, increase size of the buffer.
            continue;
        }
    }
    if(v.size() == 0)
        v.push_back((char*)"No COM Ports Found");
    return v;
}

bool SerialPort::SetBaudRate(int baud_rate) {
    printf("Setting baud rate %d\n", baud_rate);
    bool res = false;
    DCB dcbSerialParameters = {0};
    dcbSerialParameters.BaudRate = baud_rate;
    dcbSerialParameters.ByteSize = 8;
    dcbSerialParameters.StopBits = ONESTOPBIT;
    dcbSerialParameters.Parity = NOPARITY;
    dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;
    if (!SetCommState(handler, &dcbSerialParameters))
        std::cout << "ALERT: could not set serial port parameters\n";
    else {
        this->connected = true;
        PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
        Sleep(ARDUINO_WAIT_TIME);
        res = true;
        printf("ALERT: CHANGED PARAMETERS: %d\n", baud_rate);
    }
    return res;
}

SerialPort::SerialPort(const char *portName, int baudrate)
{
    std::cout << "Initialising Serial Port" << std::endl;
    this->connected = false;

    this->handler = CreateFileA(static_cast<LPCSTR>(portName),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if (this->handler == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            std::cout << "ERROR: Handle was not attached.Reason : " << portName << " not available\n";
        }
        else
        {
            std::cout << "ERROR: " << GetLastError() << "\n";
        }
    }
    else
    {
        DCB dcbSerialParameters = {0};

        if (!GetCommState(this->handler, &dcbSerialParameters))
        {
            std::cout << "Failed to get current serial parameters\n";
        }
        else
        {
            dcbSerialParameters.BaudRate = baudrate;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(handler, &dcbSerialParameters))
            {
                std::cout << "ALERT: could not set serial port parameters\n";
            }
            else
            {
                this->connected = true;
                PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }
    std::cout << "Serial Port Initialized\n-----------------\n\n" << std::endl;
}

SerialPort::~SerialPort()
{
    this->connected = false;
    CloseHandle(this->handler);
}

// Reading bytes from serial port to buffer;
// returns read bytes count, or if error occurs, returns 0
int SerialPort::readSerialPort(const char *buffer, unsigned int buf_size)
{
    DWORD bytesRead{};
    unsigned int toRead = 0;

    ClearCommError(this->handler, &this->errors, &this->status);

    if (this->status.cbInQue > 0)
    {
        if (this->status.cbInQue > buf_size)
        {
            toRead = buf_size;
        }
        else
        {
            toRead = this->status.cbInQue;
        }
    }

    memset((void*) buffer, 0, buf_size);

    if (ReadFile(this->handler, (void*) buffer, toRead, &bytesRead, NULL))
    {
        return bytesRead;
    }

    return 0;
}

// Sending provided buffer to serial port;
// returns true if succeed, false if not
bool SerialPort::writeSerialPort(const char *buffer, unsigned int buf_size)
{
    DWORD bytesSend;

    if (!WriteFile(this->handler, (void*) buffer, buf_size, &bytesSend, 0))
    {
        ClearCommError(this->handler, &this->errors, &this->status);
        return false;
    }
    
    return true;
}

// Checking if serial port is connected
bool SerialPort::isConnected()
{
    if (!ClearCommError(this->handler, &this->errors, &this->status))
    {
        this->connected = false;
    }

    return this->connected;
}

void SerialPort::closeSerial()
{
    CloseHandle(this->handler);
}

uint8_t SerialPort::read() {
    char c;
    readSerialPort(&c, 1);
    return c;
}

int SerialPort::flush() {
    //return PurgeComm(this->handler, PURGE_TXABORT);
    return true;
}

bool SerialPort::available() {
    COMSTAT comstat;
    DWORD dwErrFlags;
    ClearCommError(this->handler, &dwErrFlags, &comstat);
    if(comstat.cbInQue > 0)
        return true;
    return false;
}

bool SerialPort::write(uint8_t x) {
    return print(x);
}

bool SerialPort::write(const char * x) {
    return writeSerialPort(x, strlen(x));
}

bool SerialPort::print(string& x) {
    return writeSerialPort(x.c_str(), x.length()+1);
}

bool SerialPort::print(char x) {
    return writeSerialPort(&x, 1);
}

bool SerialPort::print(const char* x) {
    return writeSerialPort(x, strlen(x));
}