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


int main(/* int argc, char *argv[] */)
{
    cout << "Establishing database connection ..." << endl;
    RTDBConn DBC("LedEmergencyStatePublisher", 10.0);
    cout << "done" << endl << endl;

    ArduinoData ard_rtdb_obj(DBC, "ArduinoData");

    PatternGenerator pg(ard_rtdb_obj);

    int display_counter;

    try {
        cout << "Searching RTDB object \"ArduinoData\"... ";
        ard_rtdb_obj.RTDBSearchWait("ArduinoData", 0, 0, RTDB_SEARCH_WAIT_TIMEOUT);
        cout << "done" << endl;
    }
    catch(DBError err)
    {
        cerr << "main: Error when searching RTDB object: " << err.what() <<  endl;
    }

    // set both strip colors to zero with low pixel priority
    pg.setColor(0x00, 0x00, 0x00, 1);
    pg.setFillPattern();
    pg.setBattVoltageFillPattern();

    cout << "Entering loop and publishing Emergency state, if Emergency Stop active" << endl;

    while (1) {

        display_counter++;

        try {
            ard_rtdb_obj.RTDBRead();
            //cout << "done" << endl;



            if (display_counter%50000 == 0) {
                if (ard_rtdb_obj.subobj_p->emergency_state) {
                    cout << "Emergency Stop Active!" << endl;
                    // set color to red with max priority
                    pg.setColor(0x05, 0x00, 0x00, 255);
                    pg.setFillPattern();
                }
                else {
                    cout << "System OK!" << endl;

                }
            }
        }
        catch(KogniMobil::DBError err)
        {
            cerr << "main: Error when reading from RTDB object: " << err.what() <<  endl;
            cerr << "Is arduino_led_interface running?" << endl;
        }

        pg.updateStrip();

        //cout << "main: Write to RTDB ...";
        try {
            ard_rtdb_obj.RTDBWrite();
            //cout << "done" << endl;
            DBC.CycleDone();
            if (display_counter%50000 == 0) cout << ".\n";
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
