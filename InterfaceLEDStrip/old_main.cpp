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

#define RTDB_WAIT_NEXT_TIMEOUT  20.0     // max wait for update of database object in s

/*
* 1. source setup.sh
* 2. rtdb-start
* 3. multiplus --device /dev/multiplus
* 4. IceServerViewer & GuiViewer
*/

// TODO dummy typedef, usually from ArduinoData.h
/*
typedef struct {
    std::array< std::array<char, PATTERN_DEPTH>, NUMPIXELS> pattern;
    bool emergency_state = false;
} subobj_arduino_t;
*/

void databaseThread(mutex &ard_mutex, arduino *ard, ArduinoStruct *shared_data, ArduinoData *rtdb_obj, RTDBConn *DBC)
{
    cout << "done" << endl;

    ArduinoStruct data;

    while(1) {
        try{
                cout << "main:databaseThread: wating for RTDB read... ";
                rtdb_obj->RTDBReadWaitNext(DBC->getTimestamp(), RTDB_WAIT_NEXT_TIMEOUT);
                cout << "done" << endl << endl;

                // get data from rtdb_object
                data = rtdb_obj->getData();

                // std::lock_guard<std::mutex> guard(ard_mutex);
                ard_mutex.lock();
                // ----------------------------------------------------
                // read or write to protected memory here
                ard->updatePattern(data.pattern, shared_data->pattern);    // update a pixel if its new priority is higher than the current one
                data.emergency_state = shared_data->emergency_state;       // write emergency state to rtdb object

                // write into rtdb_obj memory
                rtdb_obj->setData(&data);

                // emergency state can be reset from here
                // rtdb_subobj->emergency_state = ard->setEmergencyState();
                // ----------------------------------------------------
                ard_mutex.unlock();

                // DEBUG reading to fast?
                // usleep(1000*1);
                DBC->CycleDone();
            }
        catch(KogniMobil::DBError err)
        {
            cerr << "main:databaseThread: Waiting for RTDB object update. Timeout after " << RTDB_WAIT_NEXT_TIMEOUT << " seconds" << endl;
        }
    }
}

void arduinoThread(mutex &ard_mutex, arduino *ard, ArduinoStruct *shared_data)
{
    cout << "done" << endl;
    while(1) {

        ard_mutex.lock();
        // ----------------------------------------------------
        // read or write to protected memory here
        ard->setPattern(shared_data->pattern);
        ard->decreasePrio(shared_data->pattern);
        shared_data->emergency_state = ard->getEmergencyState();
        // ----------------------------------------------------
        ard_mutex.unlock();

        ard->pattern2color_buf();
        ard->write();

        // sleep for 10ms
        usleep(1000*10);
    }
}

int main()
{

    const char *deviceName = "/dev/ttyACM0";
    arduino ard(deviceName);
    std::mutex ard_mutex;

    cout << "Establishing database connection ..." << endl;
    RTDBConn DBC("Arduino", 10.0);
    cout << "done" << endl << endl;

    // Create rtdb database object with name ArduinoData
    ArduinoData rtdb_obj(DBC, "ArduinoData");
    rtdb_obj.RTDBInsert();

    // read just created object
    rtdb_obj.RTDBRead();

    // Init shared data object for both threads
    ArduinoStruct shared_data;
    // get just created (empty) data from arduino rtdb object
    //shared_data = rtdb_obj.getData();
    shared_data.color = rtdb_obj.getData().color;

    sleep(1000);

    // start two parallel threads
    cout << "Starting database thread... ";
    std::thread dT(databaseThread, std::ref(ard_mutex), &ard, &shared_data, &rtdb_obj, &DBC);

    cout << "Starting arduino thread ...";
    std::thread aT(arduinoThread, std::ref(ard_mutex), &ard, &shared_data);

    dT.join();
    aT.join();

}



