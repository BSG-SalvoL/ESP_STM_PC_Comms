#ifndef __SIMHUBFEEDER__
#define __SIMHUBFEEDER__

#include "./winLibs/stdafx.h"
#include "./winLibs/SerialPort.hpp"
#include "./SH_Helpers/ArqSerial.h"
#include "./SH_Helpers/SHdevSpecs.h"

#define JSON_DIAGNOSTICS 1
#include "./utilLibs/json.hpp"

#include <chrono>
#include <bitset>
#include "vJoyFeeder.h"
#include "DataStructs.h"

#define ArduinoNameRX "\\\\.\\COM12"
#define SH_ERROR_SUCCESS 1
#define SH_ERROR_FAILED -1
#pragma pack(1)

using json = nlohmann::json;
using namespace nlohmann::literals;

void idle(bool critical) {}//shCustomProtocol.idle(); }
DWORD WINAPI SH_Commands(LPVOID lpParam);

class SimHubFeeder
{
private:
    SerialPort *Serial;
    SerialPort *Arduino;
    ARQSerial *arqserial;
    char SerialBuffer[255];
    std::vector<const char*> log;
    bool SH_CONNECTED;
    SimHubFeeder()
    {
        vjoyfeeder = NULL;
        SH_CONNECTED = false;
    }
    HANDLE mthread;
public:
    string actualmsg, resmsg;
    LCD_DATA_V2 parsedData;
    VolanteData v_data;
    VJoyFeeder* vjoyfeeder;

    static SimHubFeeder &get_feeder()
    {
        static SimHubFeeder instance;
        return instance;
    }
    
    void add_to_log(std::string str) {push_to_log_vector(log, str);}
    std::vector<const char*> get_log() {return log;}

    void reloadSH() {
        CloseHandle(mthread);
        add_to_log(string_format("Trying Connection with: %s%s", "\\\\.\\", SIMHUB_Conf.SH_COM_PORT.c_str()).c_str());
        SerialPort* tmp = Serial;
        tmp->closeSerial();
        delete tmp;
        Serial = new SerialPort(string_format("%s%s", "\\\\.\\", SIMHUB_Conf.SH_COM_PORT.c_str()).c_str(), 2000000);
        arqserial = new ARQSerial(Serial);
        arqserial->setIdleFunction(idle);
        SH_CONNECTED = false;
        run();
        add_to_log("SimHub Thread Started");
    }

    void Connect()
    {
        add_to_log(string_format("Trying Connection with: %s%s", "\\\\.\\", SIMHUB_Conf.SH_COM_PORT.c_str()).c_str());
        Serial = new SerialPort(string_format("%s%s", "\\\\.\\", SIMHUB_Conf.SH_COM_PORT.c_str()).c_str(), 2000000);
        arqserial = new ARQSerial(Serial);
        arqserial->setIdleFunction(idle);
        Arduino = new SerialPort(ArduinoNameRX, 115200);
        SH_CONNECTED = false;
        run();
        add_to_log("SimHub Thread Started");
    }

    ARQSerial *get_Serial()
    {
        return arqserial;
    }

    void Command_SetBaudrate(uint8_t br)
    {
        push_to_log_vector(log, string_format("DEBUG: RECEIVED BAUDRATE: %d", br));
        Sleep(20);
        //Serial->SetBaudRate(115200);
        /*
        if (br == 1) this->Serial->SetBaudRate(300);
        if (br == 2) this->Serial->SetBaudRate(1200);
        if (br == 3) this->Serial->SetBaudRate(2400);
        if (br == 4) this->Serial->SetBaudRate(4800);
        if (br == 5) this->Serial->SetBaudRate(9600);
        if (br == 6) this->Serial->SetBaudRate(14400);
        if (br == 7) this->Serial->SetBaudRate(19200);
        if (br == 8) this->Serial->SetBaudRate(28800);
        if (br == 9) this->Serial->SetBaudRate(38400);
        if (br == 10) this->Serial->SetBaudRate(57600);
        if (br == 11) this->Serial->SetBaudRate(115200);
        if (br == 12) this->Serial->SetBaudRate(230400);
        if (br == 13) this->Serial->SetBaudRate(250000);
        if (br == 14) this->Serial->SetBaudRate(1000000);
        if (br == 15) this->Serial->SetBaudRate(2000000);
        if (br == 16) this->Serial->SetBaudRate(200000);
        if (br == 17) this->Serial->SetBaudRate(500000);
        */
        return;
    }

    char FlowSerialTimedRead()
    {
        return arqserial->read();
    }

    void Command_ExpandedCommandsList()
    {
        arqserial->PrintLn("mcutype");
        //arqserial->PrintLn("keepalive");
        arqserial->PrintLn();
        arqserial->flush();
    }

    void serialData_to_LCD_DATA_V2(string actualmsg) {
        std::string delimiter = ";";
        size_t pos = 0;
        std::string token;
        uint8_t counter = 0;
        string tmp = actualmsg;
        while ((pos = tmp.find(delimiter)) != std::string::npos) {
            token = tmp.substr(0, pos);
            switch(counter) {
                case 0: //SPEED 10bit
                    parsedData.speed__position &= 0x003F;
                    parsedData.speed__position |= ((uint16_t)atoi(token.c_str()) << 6);
                    break;
                case 1: //POSITION 6 bit
                    parsedData.speed__position &= 0xFFC0;
                    parsedData.speed__position |= atoi(token.c_str());
                    break;
                case 2: //Current Lap 8 bit
                    parsedData.current_lap__total_laps &= 0x00FF;
                    parsedData.current_lap__total_laps |= ((uint16_t)atoi(token.c_str()) << 8);
                    break;
                case 3: //total laps 8 bit
                    parsedData.current_lap__total_laps &= 0xFF00;
                    parsedData.current_lap__total_laps |= atoi(token.c_str());
                    break;    
                case 4: //best lap 7 + 6 + 10 bits
                    parsedData.best_lap__fuel_perc__DRS_ACTIVE__DRS__ENABLED &= 0x1ff;
                    parsedData.best_lap__fuel_perc__DRS_ACTIVE__DRS__ENABLED |= (atoi(token.substr(0, token.find(":")).c_str()) << 25);
                    token.erase(0, token.find(":")+string(":").length());
                    parsedData.best_lap__fuel_perc__DRS_ACTIVE__DRS__ENABLED |= (atoi(token.substr(0, token.find(":")).c_str()) << 19);
                    token.erase(0, token.find(":")+string(":").length());
                    parsedData.best_lap__fuel_perc__DRS_ACTIVE__DRS__ENABLED |= (atoi(token.substr(0, token.find(":")).c_str()) << 9);
                    break;
                case 5: //delta to best
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level &= 0x1ffffffffff;
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level |= (atoll(token.substr(0, token.find(":")).c_str()) << 57);
                    token.erase(0, token.find(":")+string(":").length());
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level |= (atoll(token.substr(0, token.find(":")).c_str()) << 51);
                    token.erase(0, token.find(":")+string(":").length());
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level |= (atoll(token.substr(0, token.find(":")).c_str()) << 41);
                    break;
                case 6: //alltimetobest
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level &= 0xfffffe000003ffff;
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level |= (atoll(token.substr(0, token.find(":")).c_str()) << 34);
                    token.erase(0, token.find(":")+string(":").length());
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level |= (atoll(token.substr(0, token.find(":")).c_str()) << 28);
                    token.erase(0, token.find(":")+string(":").length());
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level |= (atoll(token.substr(0, token.find(":")).c_str()) << 18);
                    break;
                case 7: //DRS ACTIVE
                    if(atoi(token.c_str()) == 0)
                        parsedData.best_lap__fuel_perc__DRS_ACTIVE__DRS__ENABLED &= 0xfffffffd;
                    else
                        parsedData.best_lap__fuel_perc__DRS_ACTIVE__DRS__ENABLED |= 0x00000002;
                    break;
                case 8: //DRS ENABLED 
                    if(atoi(token.c_str()) == 0)
                        parsedData.best_lap__fuel_perc__DRS_ACTIVE__DRS__ENABLED &= 0xfffffffe;
                    else
                        parsedData.best_lap__fuel_perc__DRS_ACTIVE__DRS__ENABLED |= 0x00000001;
                    break;  
                case 9: //fuel perc
                    parsedData.best_lap__fuel_perc__DRS_ACTIVE__DRS__ENABLED &= 0x1fc;
                    parsedData.best_lap__fuel_perc__DRS_ACTIVE__DRS__ENABLED = atoi(token.c_str()) << 2;
                    break;
                case 10: //fl
                    parsedData.tyre_temp_front &= 0x00FF;
                    parsedData.tyre_temp_front |= ((uint16_t)atoi(token.c_str()) << 8);
                    break;
                case 11: //fr
                    parsedData.tyre_temp_front &= 0xFF00;
                    parsedData.tyre_temp_front |= ((uint16_t)atoi(token.c_str()));
                    break;
                case 12: //rl
                    parsedData.tyre_temp_rear &= 0x00FF;
                    parsedData.tyre_temp_rear |= ((uint16_t)atoi(token.c_str()) << 8);
                    break;
                case 13: //rr
                    parsedData.tyre_temp_rear &= 0xFF00;
                    parsedData.tyre_temp_rear |= ((uint16_t)atoi(token.c_str()));
                    break;   
                case 14: //ers                                                                               
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level &= 0xfffffffffffff80f;
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level |= (atoi(token.c_str()) << 4);
                    break;
                case 15: //brakebias
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level &= 0xFFFFFFFFFFFc07FF;
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level |= (atoi(token.c_str()) << 11);
                    break;
                case 16: //rpm_leds
                    parsedData.gear__rpm_leds &= 0xF0;
                    parsedData.gear__rpm_leds |= atoi(token.c_str());
                    break;
                case 17: //flags
                    parsedData.brake_temp_RR__tc_level__flags &= 0xFFF8;
                    if(token.compare("GREEN"))
                        break;
                    else if(token.compare("YELLOW"))
                        parsedData.brake_temp_RR__tc_level__flags += 1;
                    else if(token.compare("BLUE"))
                        parsedData.brake_temp_RR__tc_level__flags += 2;
                    else if(token.compare("RED"))
                        parsedData.brake_temp_RR__tc_level__flags += 3;
                    else if(token.compare("BLACK"))
                        parsedData.brake_temp_RR__tc_level__flags += 4;
                    else if(token.compare("ORANGE"))
                        parsedData.brake_temp_RR__tc_level__flags += 5;
                    else if(token.compare("BN"))
                        parsedData.brake_temp_RR__tc_level__flags += 6;
                    else
                        parsedData.brake_temp_RR__tc_level__flags += 7;
                    break;
                case 18: //game running
                    parsedData.brake_temp_FL_FR_RL__game_running__NULL &= 0xFFFFFFFd;
                    parsedData.brake_temp_FL_FR_RL__game_running__NULL |= (atoi(token.c_str()) << 1);
                    break;
                case 19: //abs_level
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level &= 0xFFFFFFFFFFFFFFF8;
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level |= (atoi(token.c_str()));
                    break;
                case 20: //abs_active
                    parsedData.tyre_wear_front__tc_active__abs_active &= 0xFFFFFFe;
                    parsedData.tyre_wear_front__tc_active__abs_active |= (atoi(token.c_str()));
                    break;
                case 21: //brake_temp fl
                    parsedData.brake_temp_FL_FR_RL__game_running__NULL &= 0x003fffff;
                    parsedData.brake_temp_FL_FR_RL__game_running__NULL |= (atoi(token.c_str()) << 22);
                    break;
                case 22: //brake_temp fr
                    parsedData.brake_temp_FL_FR_RL__game_running__NULL &= 0xffc00fff;
                    parsedData.brake_temp_FL_FR_RL__game_running__NULL |= (atoi(token.c_str()) << 12);
                    break;
                case 23: //brake_temp rl
                    parsedData.brake_temp_FL_FR_RL__game_running__NULL &= 0xfffff003;
                    parsedData.brake_temp_FL_FR_RL__game_running__NULL |= (atoi(token.c_str()) << 2);
                    break;
                case 24: //brake_temp rr
                    parsedData.brake_temp_RR__tc_level__flags &= 0x003f;
                    parsedData.brake_temp_RR__tc_level__flags |= (atoi(token.c_str()) << 6);
                    break;
                case 25: //is in pit
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level &= 0xfffffffffffffff7;
                    parsedData.delta_to_best__allTime_toBestLap__brake_bias__ers_perc__isInPit__abs_level |= (atoi(token.c_str()) << 3);
                    break;
                case 26: //tc_level
                    parsedData.brake_temp_RR__tc_level__flags &= 0xffc7;
                    parsedData.brake_temp_RR__tc_level__flags |= (atoi(token.c_str()) << 3);
                    break;
                case 27: //tc_active
                    parsedData.tyre_wear_front__tc_active__abs_active &= 0xFFFd;
                    parsedData.tyre_wear_front__tc_active__abs_active |= (atoi(token.c_str()) << 1);
                    break;
                case 28: //tyre_wear fl
                    parsedData.tyre_wear_front__tc_active__abs_active &= 0x01FF;
                    parsedData.tyre_wear_front__tc_active__abs_active |= (atoi(token.c_str()) << 9);
                    break;
                case 29: //tyre_wear fr
                    parsedData.tyre_wear_front__tc_active__abs_active &= 0xFE03;
                    parsedData.tyre_wear_front__tc_active__abs_active |= (atoi(token.c_str()) << 2);
                    break;
                case 30: //tyre_wear rl
                    parsedData.tyre_wear_rear__2NULL &= 0x01FF;
                    parsedData.tyre_wear_rear__2NULL |= (atoi(token.c_str()) << 9);
                    break;
                case 31: //tyre_wear rr
                    parsedData.tyre_wear_rear__2NULL &= 0xFE03;
                    parsedData.tyre_wear_rear__2NULL |= (atoi(token.c_str()) << 2);
                    break;
                case 32: //gear
                    parsedData.gear__rpm_leds &= 0x0F;
                    parsedData.gear__rpm_leds |= (atoi(token.c_str()) << 4);
                    break;
                default:
                    add_to_log("Invalid Counter");
                    break;               
            }
            tmp.erase(0, pos + delimiter.length());
            counter++;
        }
    }

    json* serialData_to_JSON(string actualmsg) {
        std::string delimiter = ";";
        size_t pos = 0;
        std::string token;
        uint8_t counter = 0;
        string tmp = actualmsg;
        json* ptrres = new json();
        json res;
        while ((pos = tmp.find(delimiter)) != std::string::npos) {
            token = tmp.substr(0, pos);
            switch(counter) {
                case 0: //SPEED 10bit
                    res["car"]["speed"] = atoi(token.c_str());
                    break;
                case 1: //POSITION 6 bit
                    res["timings"]["position"] = atoi(token.c_str());
                    break;
                case 2: //Current Lap 8 bit
                    res["timings"]["currentLap"] = atoi(token.c_str());
                    break;
                case 3: //total laps 8 bit
                    res["timings"]["totalLaps"] = atoi(token.c_str());
                    break;    
                case 4: //best lap 7 + 6 + 10 bits
                    res["timings"]["bestLapTime"] = token;
                    break;
                case 5: //delta to best
                    res["timings"]["deltaToBest"] = token;
                    break;
                case 6: //alltimetobest
                    res["timings"]["deltaToAllTime"] = token;
                    break;
                case 7: //DRS ACTIVE
                    res["car"]["drs"]["available"] = atoi(token.c_str());
                    break;
                case 8: //DRS ENABLED 
                    res["car"]["drs"]["enabled"] = atoi(token.c_str());
                    break;  
                case 9: //fuel perc
                    res["car"]["fuel"] = atoi(token.c_str());
                    break;
                case 10: //fl
                    res["car"]["tyres"]["temperature"] = json::array({atoi(token.c_str())});
                    break;
                case 11: //fr
                    res["car"]["tyres"]["temperature"].push_back(atoi(token.c_str()));
                    break;
                case 12: //rl
                    res["car"]["tyres"]["temperature"].push_back(atoi(token.c_str()));
                    break;
                case 13: //rr
                    res["car"]["tyres"]["temperature"].push_back(atoi(token.c_str()));
                    break;   
                case 14: //ers                                                                               
                    res["car"]["ers"] = atoi(token.c_str());
                    break;
                case 15: //brakebias
                    res["controls"]["brakeBias"] = atoi(token.c_str());
                    break;
                case 16: //rpm_leds
                    res["car"]["rpm"] = atoi(token.c_str());
                    break;
                case 17: //flags
                    res["timings"]["flags"] = token;
                    break;
                case 18: //game running
                    res["system"]["gameRunning"] = atoi(token.c_str());
                    break;
                case 19: //abs_level
                    res["controls"]["abs"]["level"] = atoi(token.c_str());
                    break;
                case 20: //abs_active
                    res["controls"]["abs"]["active"] = atoi(token.c_str());
                    break;
                case 21: //brake_temp fl
                    res["car"]["brakeTemp"] = json::array({atoi(token.c_str())});
                    break;
                case 22: //brake_temp fr
                    res["car"]["brakeTemp"].push_back(atoi(token.c_str()));
                    break;
                case 23: //brake_temp rl
                    res["car"]["brakeTemp"].push_back(atoi(token.c_str()));
                    break;
                case 24: //brake_temp rr
                    res["car"]["brakeTemp"].push_back(atoi(token.c_str()));
                    break;
                case 25: //is in pit
                    res["car"]["pitLane"] = atoi(token.c_str());
                    break;
                case 26: //tc_level
                    res["controls"]["tractionControl"]["level"] = atoi(token.c_str());
                    break;
                case 27: //tc_active
                    res["controls"]["tractionControl"]["active"] = atoi(token.c_str());
                    break;
                case 28: //tyre_wear fl
                    res["car"]["tyres"]["wear"] = json::array({atoi(token.c_str())});
                    break;
                case 29: //tyre_wear fr
                    res["car"]["tyres"]["wear"].push_back(atoi(token.c_str()));
                    break;
                case 30: //tyre_wear rl
                    res["car"]["tyres"]["wear"].push_back(atoi(token.c_str()));
                    break;
                case 31: //tyre_wear rr
                    res["car"]["tyres"]["wear"].push_back(atoi(token.c_str()));
                    break;
                case 32: //gear
                    res["car"]["gear"] = token;
                    break;
                case 33: //throttle
                    res["car"]["throttle"] = atoi(token.c_str());
                    break;
                case 34: //brake
                    res["car"]["brake"] = atoi(token.c_str());
                    break;
                case 35: //CurrentGame
                    res["system"]["currentGame"] = token;
                    break;
                default:
                    add_to_log("Invalid Counter");
                    break;               
            }
            tmp.erase(0, pos + delimiter.length());
            counter++;
        }
        *ptrres = res;
        return ptrres;
    }

    void Command_CustomProtocolData(string actualmsg) {
        auto tmr_start = chrono::high_resolution_clock::now();
        static auto tmr_retry = chrono::high_resolution_clock::now();
        arqserial->Write(0x15);
        arqserial->flush();

        json shdata = *(serialData_to_JSON(actualmsg));
        shdata["packet_type"] = "SH";
        //std::cout << std::setw(4) << shdata << "\n\n";
        //std::cout << shdata.dump() << "\n\n";

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
            }*/
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
        auto tmr_end = chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> time_span = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(tmr_end - tmr_start);
        //std::cout << "TEMPO IMPIEGATO: " << time_span.count()/1000 << " ms" << std::endl;
        //arqserial->DebugPrintLn(actualmsg);
        //push_to_log_vector(log, actualmsg);
    }

    void Command_Hello() {
        arqserial->read();
        Sleep(10);
        arqserial->Print(Rev);
        arqserial->flush();
        push_to_log_vector(log, string_format("Sending: %c as revision", Rev));
    }

    void Command_ButtonsCount() {
        arqserial->Write(J_Command);
        arqserial->flush();
        //std::cout << "Sending: " << J_Command << " as J REPLY" << endl;
        push_to_log_vector(log, string_format("Sending: %d as J CMD", J_Command));
    }

    void Command_TM1638Count() {
        arqserial->Write((uint8_t)(TM1638_ENABLEDMODULES));
        arqserial->flush();
        //std::cout << "Sending: " << TM1638_ENABLEDMODULES << " as 2CMD REPLY" << std::endl;
        push_to_log_vector(log, string_format("Sending: %d as 2 CMD", TM1638_ENABLEDMODULES));
    }

    void Command_SimpleModulesCount() {
        arqserial->Write((uint8_t)(B_Command));
        arqserial->flush();
        //std::cout << "Sending: " << B_Command << " as B Command" << std::endl;
        push_to_log_vector(log, string_format("Sending: %d as B CMD", B_Command));
    }

    void Command_Acq() {
        arqserial->Write(0x03);
        arqserial->flush();
        //std::cout << "Sending: 0x03 as A CMD" << std::endl;
        push_to_log_vector(log, string_format("Sending: 0x03 as A CMD"));
    }

    void Command_DeviceName() {
        string tmp = DEVICE_NAME;
        arqserial->WriteString(tmp);
        tmp = '\n';
        arqserial->WriteString(tmp);
        arqserial->flush();
        push_to_log_vector(log, string_format("Sending: %s as N CMD",  DEVICE_NAME));
    }

    void Command_UniqueId() {
        string tmp = "1a4b15c2-784e-435c-b648-5d0095f9819b";
        arqserial->WriteString(tmp);
        char h = '\n';
        arqserial->Print(h);
        arqserial->flush();
        std::cout << "Sending: " << tmp << "\\n as I CMD" << std::endl;
        push_to_log_vector(log, string_format("Sending: %s as I CMD", tmp));
    }

    void Command_Features()
    {
        string tmp = "N";
        tmp = "G"; arqserial->WriteString(tmp); // GEAR
        tmp = "N"; arqserial->WriteString(tmp); // Name
        tmp = "I"; arqserial->WriteString(tmp); // UNIQUE ID
        tmp = "J"; arqserial->WriteString(tmp); // Additional buttons
        tmp = "P"; arqserial->WriteString(tmp); // Custom Protocol Support
        tmp = "X"; arqserial->WriteString(tmp); // Xpanded support
        tmp = "\n"; arqserial->WriteString(tmp);
        arqserial->flush();
        //std::cout << "SENDING GNIJPX\\n" << std::endl;
        push_to_log_vector(log, string_format("Sending: GNIJPX as 0 CMD"));
    }

    void Command_RGBLEDSCount() {
        arqserial->Write(_4_Command);
        arqserial->flush();
        //std::cout << "Sending: 0x00 as 4 CMD"  << std::endl;
        push_to_log_vector(log, string_format("Sending: %d as 4 CMD", _4_Command));
    }

    void Command_MCUType() {
        arqserial->Print((char)Signature1);
        arqserial->Print((char)Signature2);
        arqserial->Print((char)Signature3);
        arqserial->flush();
        //std::cout << "Sending: " << (char)Signature1 << 
        //(char)Signature2 << (char)Signature3 << std::endl;
        push_to_log_vector(log, string_format("Sending: %c %c %c as CMD X -> MCUTYPE", (char)Signature1, (char)Signature2, (char)Signature3));
    }

    void Command_GearData(char gear) {
        parsedData.gear__rpm_leds &= 0x0F;
        parsedData.gear__rpm_leds |= (atoi(&gear) << 4);
        //std::cout << "GEAR: " << std::dec << gear << std::endl;
        push_to_log_vector(log, string_format("Sending: %d as G CMD", atoi(&gear)));
    }

    void parseBuffer() {
        for (int i = 0; i < arqserial->DataBuffer.size(); i++) {
            cout << "Buffer[" << i << "]: " << std::hex << arqserial->DataBuffer[i] << std::dec << std::endl;
        }
    }

    HANDLE run()
    {
        DWORD threadID;
        mthread = CreateThread(
            NULL,
            0,
            SH_Commands,
            this,
            0,
            &threadID);
        if (mthread == NULL)
        {
            printf("%s", "Error Creating SimHub Handler Thread");
            exit(-4);
        }
        return mthread;
    }

    bool Is_Serial_Connected() {
        if(Serial->isConnected())
            return true;
        return false;
    }

    bool Is_SH_Connected() {
        return SH_CONNECTED;
    }

    void setConnected(bool connected) {
        SH_CONNECTED = connected;
    }
};

DWORD WINAPI SH_Commands(LPVOID lpParam)
{
    SimHubFeeder* feeder = (SimHubFeeder*)lpParam;
    ARQSerial arqserial = *feeder->get_Serial();
    char loop_opt;
    uint8_t err_counter = 0;
    while (true)
    {
        if (arqserial.Available() > 0 || 1 == 1)
        {
            uint8_t cc = arqserial.read();
            if (cc == Header)
            {
                err_counter = 0;
                feeder->setConnected(true);
                // Read command
                loop_opt = (char)arqserial.read();
                //cout << "\n----------------------------------\n" << "Command Received: " << loop_opt << endl;
                if (loop_opt == '1') feeder->Command_Hello();
                else if (loop_opt == '8') {
                    uint8_t cmd = (char)arqserial.read();
                    feeder->Command_SetBaudrate(cmd);
                }
                else if (loop_opt == 'J') feeder->Command_ButtonsCount();
                else if (loop_opt == '2') feeder->Command_TM1638Count();
                else if (loop_opt == 'B') feeder->Command_SimpleModulesCount();
                else if (loop_opt == 'A') feeder->Command_Acq();
                else if (loop_opt == 'N') feeder->Command_DeviceName();
                else if (loop_opt == 'I') feeder->Command_UniqueId();
                else if (loop_opt == '0') feeder->Command_Features();
                else if (loop_opt == '4') feeder->Command_RGBLEDSCount();
                else if (loop_opt == 'G') {
                    uint8_t cmd = (char)arqserial.read();
                    feeder->Command_GearData(cmd);
                }
                else if (loop_opt == 'P') {
                    string actualmsg = arqserial.ReadStringUntil('\n');
                    feeder->Command_CustomProtocolData(actualmsg);
                }
                else if (loop_opt == 'X')
                {
                    string xaction = arqserial.ReadStringUntil(' ', '\n');
                    //std::cout << "RECEIVED: " << xaction << " after X CMD" << std::endl;
                    if (xaction == "list") feeder->Command_ExpandedCommandsList();
                    else if (xaction == "mcutype") feeder->Command_MCUType();
                }
                else 
                    std::cout << "UNRECOGNIZED CMD RECEIVED=" << std::hex << (int)loop_opt << std::dec << std::endl;
            }
            else {
                err_counter++;
                if(err_counter >= 1000)
                    feeder->setConnected(false);
                //std::cout << "Read Non Header Byte=" << std::hex << (int)cc << std::dec << '\n';
            }
        }
    }
    feeder->add_to_log("Exiting From Thread...");
    return 0;
}

#endif