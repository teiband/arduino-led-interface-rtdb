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
#include <math.h>

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

    cout << "Establishing database connection ..." << endl;
    RTDBConn DBC("LedBatteryStatePublisher", 10.0);
    cout << "done" << endl << endl;

    ArduinoData ard_rtdb_obj(DBC, "ArduinoData");
    MultiplusData multiplus_rtdb_obj(DBC, "MultiplusData");

    PatternGenerator pg(ard_rtdb_obj);

    try {
        cout << "Searching RTDB object \"ArduinoData\"... ";
        ard_rtdb_obj.RTDBSearchWait("ArduinoData", 0, 0, RTDB_SEARCH_WAIT_TIMEOUT);
        cout << "done" << endl;
        cout << "Searching RTDB object \"MultiPlusData\"... ";
        multiplus_rtdb_obj.RTDBSearchWait("MultiPlusData", 0, 0, RTDB_SEARCH_WAIT_TIMEOUT);
        cout << "done" << endl;
    }
    catch(KogniMobil::DBError err)
    {
        cerr << "main: Error when searching RTDB object: " << err.what() <<  endl;
    }

    // set long led strip colors to zero
    pg.clear();

    cout << "Entering loop and publishing battery state" << endl;

    while (1) {

        multiplus_rtdb_obj.RTDBRead();

        if (!multiplus_rtdb_obj.subobj_p->On.mains) {
            if (verbose_mode) cout << "Multiplus Mains is not connected" << endl;

            // voltage
            float V = multiplus_rtdb_obj.subobj_p->if0_DC_Voltage;

            // state of charge model
            float SoC = 0.37*(atan(V-48.0)+1.3);

            pg.processBattVoltage(SoC);

            //pg.setBattVoltage(multiplus_rtdb_obj.subobj_p->if0_DC_Voltage, MIN_VOLTAGE, MAX_VOLTAGE);
        }
        else {
            if (verbose_mode) cout << "Multiplus Mains is connected" << endl;
            // set full bar to blue
            pg.setColor(0x00, 0x00, 0x10, 127);
            pg.setBattVoltageFillPattern();
        }

        pg.updateVoltageStrip();

        if (verbose_mode) cout << "main: Write to RTDB ...";
        try {
            ard_rtdb_obj.RTDBWrite();
            if (verbose_mode) cout << "done" << endl;
            DBC.CycleDone();
        }
        catch(KogniMobil::DBError err)
        {
            cerr << "main: Error when writing to RTDB object: " << err.what() <<  endl;
            cerr << "Is arduino_led_interface running?" << endl;
        }

        usleep(1000*500);

    }
    return 0;
}
