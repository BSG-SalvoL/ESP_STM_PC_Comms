#pragma once

#include <iostream>
#include <string>
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#include <WinSock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include "winLibs/SerialPort.hpp"
#include "vJoyFeeder.h"
#include "utilLibs/json.hpp"
#include "utils.h"
#include "config.h"

#define VOL_SERIAL 0
#define VOL_UDP 1
#define VOL_TCP 2
#define VOL_BLE 3

#define VOL_UDP_PORT_RX 23230
#define VOL_UDP_PORT_TX 23231

#define VOL_SERIAL_NAME_RX "\\\\.\\COM12"

using json = nlohmann::json;
using namespace nlohmann::literals;
using namespace std;

DWORD WINAPI BLE_BOARD_LOOP(LPVOID lpParam);

class SG_VolBoard {
    protected:
        VJoyFeeder joystick = VJoyFeeder::getInstance();
        uint8_t connectionType;
        std::vector<const char*> log;
        HANDLE mthread;
    public:
        json SH_JSON;
        bool notify; 
        SG_VolBoard() {
            connectionType = VOL_UDP;
            this->notify = false;
            push_to_log_vector(log, "Class Initialized");
        }
        SG_VolBoard(uint8_t connectionType) {
            this->connectionType = connectionType;
            this->notify = false;
            push_to_log_vector(log, "Class Initialized");
        }
        void add_to_log(std::string str) {push_to_log_vector(log, str);}
        std::vector<const char*> get_log() {return log;}
        void setSH(json data) {
            SH_JSON = data;
        }

        virtual bool is_connected() = 0;
        virtual string* getNames() = 0;
        virtual void setNames(string* names) = 0;
        virtual void reload(string* com_ports) = 0; 
        virtual HANDLE run() = 0;
        virtual bool send(json data) = 0;
        virtual json receive() = 0;
};

class SG_VolBoardUDP : public SG_VolBoard {
    private:
        string destIP = "";
        string bindAddress = "0.0.0.0";
        uint16_t txport, rxport;
    public:
        SG_VolBoardUDP(uint16_t txport, uint16_t rxport) : SG_VolBoard(VOL_UDP) {
            this->txport = txport;
            this->rxport = rxport;
        }
        HANDLE run() {
            return mthread;
        }
        bool send(json jdata) {
            if(destIP == "")
                return false;
            try {
                string jstring = jdata.dump();
                sockaddr_in dest;
                sockaddr_in local;
                WSAData data;
                WSAStartup(MAKEWORD(2, 2), &data);
                dest.sin_family = AF_INET;
                inet_pton(AF_INET, destIP.c_str(), &dest.sin_addr.s_addr);
                dest.sin_port = htons(23231);
                SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                //bind(s, (sockaddr *)&local, sizeof(local));
                sendto(s, jstring.c_str(), jstring.length(), 0, (sockaddr *)&dest, sizeof(dest));
                closesocket(s);
                WSACleanup();
                return true;
            }
            catch (const std::exception& e) {
                return false;
            }
        }
        json receive() {
            json res;
            return res;
        }
};

class SG_VolBoardSerial : public SG_VolBoard {
    private:
        SerialPort* serial;
    public:
        SG_VolBoardSerial(string COM_Name, uint32_t baudrate) : SG_VolBoard(VOL_SERIAL) {
            serial = new SerialPort(COM_Name.c_str(), baudrate);
        }
        HANDLE run() {
            return mthread;
        }
        bool send(json data) {
            return false;
        }
        json receive() {
            json res;
            return res;
        }
};

class SG_VolBoardBLE : public SG_VolBoard {
    private:
        SerialPort *SendSerial, *RecvSerial;
        string sendName, recvName;
    public:
        SG_VolBoardBLE(string SEND_COM_NAME, string RECV_COM_NAME, uint32_t baudrate) : SG_VolBoard(VOL_BLE) {
            SendSerial = new SerialPort(SEND_COM_NAME.c_str(), baudrate);
            RecvSerial = new SerialPort(RECV_COM_NAME.c_str(), baudrate);
            sendName = SEND_COM_NAME;
            recvName = RECV_COM_NAME;
        }

        bool is_connected() {
            if (SendSerial->isConnected() && RecvSerial->isConnected())
                return true;
            return false;
        }

        string* getNames() {
            string* names = new string[2];
            names[0] = sendName;
            names[1] = recvName;
            return names;
        }
        void setNames(string* names) {
            this->sendName = names[0];
            this->recvName = names[1];
        }
        HANDLE run()
        {
            DWORD threadID;
            mthread = CreateThread(
                NULL,
                0,
                BLE_BOARD_LOOP,
                this,
                0,
                &threadID);
            if (mthread == NULL)
            {
                printf("%s", "Error Creating VolBoard Handler Thread");
                exit(-4);
            }
            return mthread;
        }
        void reload(string* com_ports) {
            CloseHandle(mthread);
            add_to_log(string_format("Trying Connection with: %s%s and %s%s", "\\\\.\\", com_ports[0].c_str(), "\\\\.\\", com_ports[1].c_str()));
            
            SerialPort* tmp = SendSerial;
            tmp->closeSerial();
            delete tmp;
            tmp = RecvSerial;
            tmp->closeSerial();
            delete tmp;

            SendSerial = new SerialPort(string_format("%s%s", "\\\\.\\", com_ports[0].c_str()).c_str(), 115200);
            RecvSerial = new SerialPort(string_format("%s%s", "\\\\.\\", com_ports[1].c_str()).c_str(), 115200);
            run();
            add_to_log("New BLE VOL BOARD Thread Started");
        }
        bool Is_Serial_Connected() {
            if(SendSerial->isConnected() && RecvSerial->isConnected())
                return true;
            return false;
        }
        bool send(json data) {
            if(SendSerial->isConnected())
                SendSerial->writeSerialPort(data.dump().c_str(), data.dump().size());
            return false;
        }
        json receive() {
            json res;
            char* SerialBuffer = new char[1024];
            if(RecvSerial->isConnected() && RecvSerial->available()) {
                RecvSerial->readSerialPort(SerialBuffer, 1024);
                if (strstr(SerialBuffer, "NO_VL_DATA")) {}
                else {
                    res = json::parse(SerialBuffer);
                    push_to_log_vector(log, res.dump().c_str());
                }
            }
            return res;
        }
};

DWORD WINAPI BLE_BOARD_LOOP(LPVOID lpParam) {
    SG_VolBoard* board = (SG_VolBoard*)lpParam;
    while(true) {
        try {
            if(board->notify) {
                board->send(board->SH_JSON);
                board->notify = false;
            }
        }
        catch (const std::exception& e) {
            // Code to handle the exception
            std::cout << "Exception caught on Send: " << e.what() << std::endl;
        }
        try {
            board->receive();
        }
        catch (const std::exception& e) {
            // Code to handle the exception
            std::cout << "Exception caught on Receive: " << e.what() << std::endl;
        }
            
    }
    return 0;
}

/*


        if(Arduino->isConnected()) {
            actualmsg += "\n";
            Arduino->writeSerialPort(actualmsg.c_str(), actualmsg.length());
            if(Arduino->available()) {
                Arduino->readSerialPort(SerialBuffer, 254);
                //cout << "RECEIVED BACK: " << SerialBuffer << endl;
                if (strstr(SerialBuffer, "NO_VL_DATA")) {}
                else {
                    vjoyfeeder->iReport.lButtons = ~v_data.mcp_btn;
                    memcpy((void*)&v_data, (void*)SerialBuffer, sizeof(v_data));
                    system("cls");
                    cout << "BTN: " << std::bitset<16>(v_data.mcp_btn) << "\n";
                    cout << "ENC: " << std::bitset<16>(v_data.mcp_enc) << "\n";
                    cout << "EXT: " << std::bitset<16>(v_data.mcp_ext) << "\n";
                    cout << "RW1: " << std::bitset<16>(v_data.rw1) << "\n";
                    cout << "RW2: " << std::bitset<16>(v_data.rw2) << "\n";
                    cout << "RW3: " << std::bitset<16>(v_data.rw3) << "\n";
                    cout << "RW4: " << std::bitset<16>(v_data.rw4) << "\n";
                    cout << "RW5: " << std::bitset<16>(v_data.rw5) << "\n";
                    cout << "RW6: " << std::bitset<16>(v_data.rw6) << "\n";
                    cout << "CLU: " << std::bitset<16>(v_data.clutch) << "\n";
                    cout << "TG0: " << std::bitset<16>(v_data.tg0) << "\n";
                }
            }
            /*
            while(Arduino->available()) {
                resmsg += Arduino->read();
                if(resmsg[resmsg.size() - 1] == '\n') {
                    cout << "RECEIVED BACK: " << resmsg << std::endl;
                    resmsg = "";
                    break;
                }
            } //DOUBLE COMMENT ENDS HERE
        }
        else {
            std::chrono::duration<double, std::micro> retry_span = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(tmr_start - tmr_retry);
            double rtr = retry_span.count()/1000;
            if(rtr > 2000) {
                SerialPort * tmp = Arduino;
                Arduino = new SerialPort(ArduinoNameRX, 115200);
                delete tmp;
                tmr_retry = chrono::high_resolution_clock::now();
            }
        }
*/