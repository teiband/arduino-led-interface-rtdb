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
    RTDBConn DBC("PatternGenerator", 5.0);
    cout << "done" << endl << endl;

    ArduinoData rtdb_obj(DBC, "ArduinoData");

    PatternGenerator pg(rtdb_obj);

    cout << "Searching RTDB object \"ArduinoData\"... ";
    rtdb_obj.RTDBSearchWait("ArduinoData");

    cout << "done" << endl;

    cout << "Entering loop and generating some patterns" << endl;
    while (1) {

        // TODO implement pattern here, its directly written into pattern_t pattern given in constructor from rtdb object
        pg.setColor(0x10, 0x00, 0x00);
        pg.setFillPattern();


        // TODO do not forget!!!
        pg.update();

        cout << "Write to RTDB ...";
        rtdb_obj.RTDBWrite();
        cout << "done" << endl;

        sleep(1);

        // wait until cycle done
        // (signal for ReadWaitNext() in Arduino main)
        DBC.CycleDone();
    }
    return 0;
}
