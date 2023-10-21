#pragma once
#include "./winLibs/stdafx.h"
#include "./VJ_Helpers/public.h"
#include "./VJ_Helpers/vjoyinterface.h"
#include "config.h"
#include "utils.h"
#include <vector>
#include <string>

DWORD WINAPI handleJoystick(LPVOID lpParam);

class VJoyFeeder;

typedef struct TH_data {
    JOYSTICK_POSITION_V2* rprt;
    std::vector<const char*>* log;
    int counter;
    VJoyFeeder* feeder;
} TH_data;

class VJoyFeeder
{
private:
    int ffb_direction;
    int ffb_strenght;
    int serial_result;
    int stat = 0;
    std::vector<const char*> log; 

    static TH_data data;

    VJoyFeeder()
    {
        ffb_direction = 0;
        ffb_strenght = 0;
        serial_result = 0;
        VJoyFeeder::acquired = false;
        data.rprt = &this->iReport;
        data.log = &this->log;
        data.counter = 0;
        data.feeder = this;
        check_vjoydriver();
        acquireDevice();
        iReport.bDevice = VJOY_Conf.VJOY_DEVICE_ID;
        //run();
    }

    void check_vjoydriver() {
        WORD VerDll, VerDrv;
        if(DriverMatch(&VerDll, &VerDrv)) {
            push_to_log_vector(log, string_format("OK - vJoy driver ver: %04x match dll version: %04x", VerDrv, VerDll));
        }
        else {
            push_to_log_vector(log, string_format("ERROR - vJoy driver ver: %04x does not match dll version: %04x", VerDrv, VerDll));
        }
    }

    bool getDevStatus() {
        VjdStat status = GetVJDStatus(VJOY_Conf.VJOY_DEVICE_ID);
        bool res = true;
        switch (status)
        {
        case VJD_STAT_OWN: 
            push_to_log_vector(log, string_format("vJoy device %d is already owned by this feeder", VJOY_Conf.VJOY_DEVICE_ID));
            break;
        case VJD_STAT_FREE:
            push_to_log_vector(log, string_format("vJoy device %d is free\n", VJOY_Conf.VJOY_DEVICE_ID));
            break;
        case VJD_STAT_BUSY:
            push_to_log_vector(log, string_format("vJoy device %d is already owned by another feeder\nCannot continue\n", VJOY_Conf.VJOY_DEVICE_ID));
        case VJD_STAT_MISS:
            push_to_log_vector(log, string_format("vJoy device %d is not installed or disabled\nCannot continue\n", VJOY_Conf.VJOY_DEVICE_ID));
            res = false;
        default:
            push_to_log_vector(log, string_format("vJoy device %d general error\nCannot continue\n", VJOY_Conf.VJOY_DEVICE_ID));
            res = false;
        };
        return res;
    }

    void acquireDevice()
    {
        if (!vJoyEnabled())
        {
            push_to_log_vector(log, "Function vJoyEnabled Failed - make sure that vJoy is installed and enabled");
            int dummy = getchar();
            stat = -2;
            //mExit(VJOY_Conf.VJOY_DEVICE_ID, -6);
        }
        else
        {
            char* tchar_tmp = (char*)(GetvJoyManufacturerString());
            push_to_log_vector(log, "Vendor: " + std::string(tchar_tmp));
            tchar_tmp = (char*)(GetvJoyProductString());
            push_to_log_vector(log, "Product: " + std::string(tchar_tmp));
            tchar_tmp = (char*)(GetvJoySerialNumberString());
            push_to_log_vector(log, "Version: " + std::string(tchar_tmp));
        };

        // Get the status of the vJoy device before trying to acquire it
        if(!getDevStatus()) {
            push_to_log_vector(log, string_format("vJoy device %d cannot be acquired", VJOY_Conf.VJOY_DEVICE_ID));
            RelinquishVJD(VJOY_Conf.VJOY_DEVICE_ID);
            return;
        }

        // Acquire the vJoy device
        if (!AcquireVJD(VJOY_Conf.VJOY_DEVICE_ID))
        {
            push_to_log_vector(log, string_format("Failed to acquire vJoy device number %d.\n", VJOY_Conf.VJOY_DEVICE_ID));
            int dummy = getchar();
            stat = -1;
            //mExit(VJOY_Conf.VJOY_DEVICE_ID, -1);
        }
        else {
            push_to_log_vector(log, string_format("Acquired device number %d - OK\n", VJOY_Conf.VJOY_DEVICE_ID));
            VJoyFeeder::acquired = true;
        }
    }

    void mExit(UINT ID, int err_code) {
	    RelinquishVJD(ID);
	    exit(err_code);
    }
    
    HANDLE run() {
        DWORD threadID;
        HANDLE mthread = CreateThread(
            NULL,
            0,
            handleJoystick,
            &data,
            0,
            &threadID
        );
        if(mthread == NULL) {
            push_to_log_vector(log, string_format("%s", "Error Creating Joystick Handler Thread"));
            exit(-4);
        }
        return mthread;
    }
public:
    JOYSTICK_POSITION_V2 iReport; // The structure that holds the full position data
    static bool acquired;
    static VJoyFeeder& get_feeder()
    {
        static VJoyFeeder instance;
        return instance;
    }
    static bool is_acquired() {
        return VJoyFeeder::acquired;
    }

    static void unacquire() {
        if(GetVJDStatus(VJOY_Conf.VJOY_DEVICE_ID) == VJD_STAT_OWN)
            RelinquishVJD(VJOY_Conf.VJOY_DEVICE_ID); 
        VJoyFeeder::acquired = false;
    }

    void reload() {
        VJoyFeeder::acquired = false;
        push_to_log_vector(log, string_format("Relinquishing device %d", VJOY_Conf.VJOY_DEVICE_ID));
        if(GetVJDStatus(VJOY_Conf.VJOY_DEVICE_ID) == VJD_STAT_OWN)
            RelinquishVJD(VJOY_Conf.VJOY_DEVICE_ID);
        
        if(!getDevStatus()) {
            push_to_log_vector(log, string_format("vJoy device %d cannot be acquired", VJOY_Conf.VJOY_DEVICE_ID));
            RelinquishVJD(VJOY_Conf.VJOY_DEVICE_ID);
            return;
        }

        // Acquire the vJoy device
        if (!AcquireVJD(VJOY_Conf.VJOY_DEVICE_ID))
            push_to_log_vector(log, string_format("Failed to acquire vJoy device number %d.\n", VJOY_Conf.VJOY_DEVICE_ID));
        else {
            push_to_log_vector(log, string_format("Acquired device number %d - OK\n", VJOY_Conf.VJOY_DEVICE_ID));
            VJoyFeeder::acquired = true;
        }
    }

    std::vector<const char*> getVJoyDevices() {
        std::vector<const char*> devices;
        int numDev = 0;
        GetvJoyMaxDevices(&numDev);
        push_to_log_vector(log, string_format("Max Devices: %d", numDev));
        for(int i = 0; i < numDev; i++) {
            if(isVJDExists(i)) {
                push_to_log_vector(devices, string_format("Device %d\0", i));
                push_to_log_vector(log, string_format("Device %d Found", i));
            }
        }
        if(devices.size() == 0)
            devices.push_back((const char*)"No Devices Found");
        return devices;
    }

    void add_to_log(std::string str) {push_to_log_vector(log, str);}
    std::vector<const char*> get_log() {return log;}
    TH_data get_data() {return data;}

    bool update() {
        //push_to_log_vector(log, string_format("Feeding vJoy device"));
        data.counter++;
        if (!UpdateVJD(VJOY_Conf.VJOY_DEVICE_ID, data.rprt))
        {
            push_to_log_vector(log, string_format("Feeding vJoy device number %d failed - try to enable device then press enter\n", VJOY_Conf.VJOY_DEVICE_ID));
            reload();
            return false;
        }
        return true;
    }

};

bool VJoyFeeder::acquired = false;
TH_data VJoyFeeder:: data = *(new TH_data);

DWORD WINAPI handleJoystick(LPVOID lpParam) {
    TH_data data = *(TH_data*)lpParam;
    while (true) {
        push_to_log_vector(*data.log, string_format("Feeding vJoy device"));
        data.counter++;
        if (!UpdateVJD(VJOY_Conf.VJOY_DEVICE_ID, data.rprt))
        {
            push_to_log_vector(*data.log, string_format("Feeding vJoy device number %d failed - try to enable device then press enter\n", VJOY_Conf.VJOY_DEVICE_ID));
            data.feeder->reload();
        }
    }
    return 0;
}