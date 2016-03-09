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

#define RTDB_SEARCH_WAIT_TIMEOUT 1.0

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
    RTDBConn DBC("PatternGenerator", 1.0);
    cout << "done" << endl << endl;

    ArduinoData rtdb_obj(DBC, "ArduinoData");

    PatternGenerator pg(rtdb_obj);

    cout << "Searching RTDB object \"ArduinoData\"... ";

    try {
       rtdb_obj.RTDBSearchWait("ArduinoData", 0, 0, RTDB_SEARCH_WAIT_TIMEOUT);
    }
    catch(KogniMobil::DBError err)
    {
        cerr << "main: Error when searching RTDB object: " << err.what() <<  endl;
    }
    cout << "done" << endl;

    cout << "Entering loop and generating some patterns" << endl;

    char r = 0;
    char g = 0;
    char b = 0;

    char max_val = 10;

    while (1) {

        // TODO implement pattern here, its directly written into pattern_t pattern given in constructor from rtdb object

        if (r > max_val) r = 0;
        if (g > max_val) g = 0;
        if (b > max_val) b = 0;

        pg.setColor((r++), (g++), (b++));
        pg.setFillPattern();       
        pg.setBattVoltageFillPattern();


        // TODO do not forget to update pattern_generator before writing to rtdb!!!
        pg.updateAll();

        sleep(0.05);

        cout << "main: Write to RTDB ...";
        try {
            rtdb_obj.RTDBWrite();
            cout << "done" << endl;
            // TODO adapt cycle time
            //sleep(0.010);
            DBC.CycleDone();
        }
        catch(KogniMobil::DBError err)
        {
            cerr << "main: Error when writing to RTDB object: " << err.what() <<  endl;
            cerr << "Is arduino_led_interface running?" << endl;
        }

    }
    return 0;
}
