/*
 * main.cpp
 *
 *  Created on: 15 Nov 2015
 *      Author: Thomas Eiband
 *
 *      thomas.eiband@tum.de
 */

#include "PatternGenerator.h"
#include <getopt.h>
#include <iostream>
#include <unistd.h> // for sleep()
#include <ArduinoData.h>
#include <MultiPlusData.h>

#define RTDB_SEARCH_WAIT_TIMEOUT 1.0
#define MIN_VOLTAGE 45.0
#define MAX_VOLTAGE 52.0

using namespace std;

#include <kogmo_rtdb.hxx>
using namespace KogniMobil;

static struct option long_options[] = {
   {"device", required_argument, 0, 'd'},
   {"verbose", no_argument, 0, 'v'},
   {0,0,0,0}
};

int main(int argc, char *argv[])
{
    cout << "Establishing database connection ..." << endl;
    RTDBConn DBC("BatteryStatePublisher", 10.0);
    cout << "done" << endl << endl;

    ArduinoData ard_rtdb_obj(DBC, "ArduinoData");

    PatternGenerator pg(ard_rtdb_obj);

    try {
        cout << "Searching RTDB object \"ArduinoData\"... ";
        ard_rtdb_obj.RTDBSearchWait("ArduinoData", 0, 0, RTDB_SEARCH_WAIT_TIMEOUT);
        cout << "done" << endl;
        /* insert rtdb object for robot movement here
        cout << "Searching RTDB object \"MultiPlusData\"... ";
        multiplus_rtdb_obj.RTDBSearchWait("MultiPlusData", 0, 0, RTDB_SEARCH_WAIT_TIMEOUT);
        cout << "done" << endl;
        */
    }
    catch(KogniMobil::DBError err)
    {
        cerr << "main: Error when searching RTDB object: " << err.what() <<  endl;
    }

    cout << "Entering loop and publishing movement state" << endl;

    while (1) {

        cout << "Sending pattern" << endl;
        pg.setColor(0x00, 0x00, 0x10);
        pg.setFillPattern();
        pg.updateStrip();
        
        cout << "Sending pattern" << endl;
        pg.setColor(0x00, 0x00, 0x10);
        pg.setFillPattern();
        pg.updateStrip();
        
        cout << "Sending pattern" << endl;
        pg.setColor(0x00, 0x00, 0x10);
        pg.setFillPattern();
        pg.updateStrip();

        cout << "main: Write to RTDB ...";
        try {
            ard_rtdb_obj.RTDBWrite();
            cout << "done" << endl;
            DBC.CycleDone();
        }
        catch(KogniMobil::DBError err)
        {
            cerr << "main: Error when writing to RTDB object: " << err.what() <<  endl;
            cerr << "Is arduino_led_interface running?" << endl;
        }

        sleep(2.0);

    }
    return 0;
}
