/*
 * main.cpp
 *
 *  Created on: 22 Nov 2015
 *      Author: Thomas Eiband
 *
 *      thomas.eiband@tum.de
 */


#include "main.h"
#include <iostream>

#include <math.h>
#include <stdint.h>
#include "arduino.h"
#include "arduino_config.h"
#include <ArduinoData.h>
#include <getopt.h>
#include <iostream>
#include <unistd.h> // for sleep()
#include <thread>
#include <mutex>
using namespace std;

#include <kogmo_rtdb.hxx>
using namespace KogniMobil;

static struct option long_options[] = {
   {"device", required_argument, 0, 'd'},
   {"verbose", no_argument, 0, 'v'},
   {0,0,0,0}
};

#define RTDB_WAIT_NEXT_TIMEOUT  2.0     // max time for update of database object in s

void databaseThread(mutex &ard_mutex, arduino *ard, ArduinoStruct *shared_data, ArduinoData *rtdb_obj, RTDBConn *DBC, bool verbose_mode)
{
    cout << "Database Thread started" << endl;

    subobj_arduino_t *data = rtdb_obj->subobj_p;

    while(1) {
        try{
                if (verbose_mode) cout << "main:databaseThread: wating for RTDB read... ";
                rtdb_obj->RTDBReadWaitNext(0 /*DBC->getTimestamp() */, RTDB_WAIT_NEXT_TIMEOUT);
                if (verbose_mode) cout << "done" << endl << endl;

                ard_mutex.lock();
                // ----------------------------------------------------
                // read or write to protected memory here (shared_data)
                /*
                 * data->pattern                    >>>>    shared_data->pattern
                 * data->voltage_pattern            >>>>    shared_data->voltage_pattern
                 * shared_data->emergency_state     >>>>    data->emergency_state
                 */

                ard->updatePattern(data->pattern, shared_data->pattern);    // update a pixel if its new priority is higher than the current one
                ard->updateVoltagePattern(data->voltage_pattern, shared_data->voltage_pattern);
                data->emergency_state = shared_data->emergency_state;       // write emergency state to rtdb object
                // std::cout << "Emergency State: " << shared_data->emergency_state << std::endl;

                // ----------------------------------------------------
                ard_mutex.unlock();

                DBC->CycleDone();
            }
        catch(KogniMobil::DBError err)
        {
            if (verbose_mode) {
                cerr << "main:databaseThread: Error: " << err.what() << endl;
                cerr << "Is any agent writing to the ArduinoData RTDB-object?" << endl;
            }
        }

    }
}

void arduinoThread(mutex &ard_mutex, arduino *ard, ArduinoStruct *shared_data, bool verbose_mode)
{
    cout << "Arduino Thread started" << endl;
    while(1) {

        ard_mutex.lock();
        // ----------------------------------------------------
        // read or write to protected memory here (shared_data)
        ard->setPattern(shared_data->pattern, shared_data->voltage_pattern);
        ard->decreasePrio(shared_data->pattern, shared_data->voltage_pattern);
        shared_data->emergency_state = ard->getEmergencyState();
        // ----------------------------------------------------
        ard_mutex.unlock();

        // copy pattern into output buffer
        ard->pattern2color_buf();
        ard->write();



        if (verbose_mode) cout << "arduinoThread: data written to arduino" << endl;
    }
}


int main(int argc, char *argv[])
{
    int o;
    int optionindex;
    bool verbose_mode = false;

    while ((o = getopt_long(argc, argv, "d:v", long_options, &optionindex)) != -1) {
        switch(o) {
            case 'v':
                verbose_mode = true;
                break;
            case '?':
                std::cerr << "Usage: " << argv[0] << " [-v     \"verbose mode\"]" << std::endl;
                exit(1);
                break;
            default:
                abort();
        }
    }

    const char *deviceName = "/dev/ttyACM1";
    arduino ard(deviceName);
    std::mutex ard_mutex;

    cout << "Establishing database connection ..." << endl;
    RTDBConn DBC("ArduinoLEDInterface", 5.0);       // TODO change update time
    cout << "done" << endl << endl;

    // Create rtdb database object with name ArduinoData
    ArduinoData rtdb_obj(DBC, "ArduinoData");
    rtdb_obj.setAllowPublicWrite(true);
    rtdb_obj.RTDBInsert();
    // Initial write to show dump in GUI
    rtdb_obj.RTDBWrite();

    ArduinoStruct shared_data;

    shared_data = *rtdb_obj.subobj_p;

    // start two parallel threads
    // cout << "Starting database thread... ";
    std::thread dT(databaseThread, std::ref(ard_mutex), &ard, &shared_data, &rtdb_obj, &DBC, verbose_mode);

    // cout << "Starting arduino thread ...";
    std::thread aT(arduinoThread, std::ref(ard_mutex), &ard, &shared_data, verbose_mode);

    dT.join();
    aT.join();

}



