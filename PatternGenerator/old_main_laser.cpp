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
#include <LaserData.h>

#include <signal.h> // necessary?

#define RTDB_SEARCH_WAIT_TIMEOUT 1.0
#define MIN_VOLTAGE 45.0
#define MAX_VOLTAGE 52.0


// Constants for Laser Positions relative to LED strip
// LED strip index 0 is on back right side of the robot
// direction of increasing index is CCW
// #define LED_STRIP_POLAR_OFFSET 5.0       // rotation of led strip index 0 from front of the robot in rad
#define LASER_REAR_POLAR_OFFSET     0.0f    // rotation relative to pixel index 0
#define LASER_FRONT_POLAR_OFFSET    M_PI    // rotation relative to LED pixel index 0
#define LASER_ANGLE_RANGE_RATIO     1.5f    // maps from 270 degress laser view to 180 degree LED strip on the robot (= 270/180)
#define LASER_MAX_RANGE             1.0f    // in meter, color intensity of the LED strip starts with 0 at this range
#define LED_MAX_INTENSITY           150.0   // maximum color intensity of the LED strip (range: 0 ... 255)

using namespace std;

#include <kogmo_rtdb.hxx>
using namespace KogniMobil;

/*
static struct option long_options[] = {
   {"device", required_argument, 0, 'd'},
   {"verbose", no_argument, 0, 'v'},
   {0,0,0,0}
};
*/

void processLaserScan(float angle, float dist, PatternGenerator &pg)
{
    // assign each laser ray to a LED on the strip
    // pass angle relative to led strip polar coordinate system

    int lin_intensity = (-LED_MAX_INTENSITY / LASER_MAX_RANGE) * dist + LED_MAX_INTENSITY;

    if (lin_intensity > LED_MAX_INTENSITY) lin_intensity = LED_MAX_INTENSITY;
    if (lin_intensity < 0) lin_intensity = 0;

    //char intensity = (0.1 / dist) * 10.0;
    //if (intensity > max_intensity) intensity = max_intensity;

    // increase intensity in blue channel if ranges get smaller
    pg.setColor(0x00, 0x00, lin_intensity);
    pg.setPixel(pg.polar2index(angle));
}


int main(/* int argc, char *argv[] */)
{
    cout << "Establishing database connection ..." << endl;
    RTDBConn DBC("LedLaserPublisher", 10.0);
    cout << "done" << endl << endl;

    ArduinoData ard_rtdb_obj(DBC, "ArduinoData");

    PatternGenerator pg(ard_rtdb_obj);

    string laser_front_rtdbname("laser_front");
    string laser_rear_rtdbname("laser_rear");

    LaserData laser_front(DBC);
    LaserData laser_rear(DBC);

    int display_counter;

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
    catch(DBError err)
    {
        cerr << "main: Error when searching RTDB object: " << err.what() <<  endl;
    }

    // set both strip colors to zero with low pixel priority
    pg.setColor(0x00, 0x00, 0x00, 1);
    pg.setFillPattern();
    pg.setBattVoltageFillPattern();

    cout << "Entering loop and publishing Laser state" << endl;

    while (1) {

        display_counter++;

        // REAR LASER
        try {
            laser_rear.RTDBRead();
            // float min_angle = laser_rear.phi(0);
            // float max_angle = laser_rear.phi(laser_rear.size()-1);
            for (unsigned int i=0; i<(laser_rear.size()); i++){
                float angle = (laser_rear.phi(i) / LASER_ANGLE_RANGE_RATIO) + LASER_REAR_POLAR_OFFSET;
                float dist = sqrt(laser_rear.local(i).x*laser_rear.local(i).x + laser_rear.local(i).y*laser_rear.local(i).y);
                processLaserScan(angle, dist, pg);
            }
        } catch(DBError err) {
            std::cerr << "Cannot read RTDB Object." << std::endl << err.what() << std::endl;
            try {
                    laser_rear.RTDBSearch(laser_rear_rtdbname.c_str());
                    laser_rear.RTDBRead();
                    std::cout << "Successfully read " << laser_rear_rtdbname << std::endl;
            } catch (DBError err) {
                    std::cerr << "Object not found." << std::endl;
                    std::cout << err.what();
            }
        }
        // FRONT LASER
        try {
            laser_front.RTDBRead();
            // float min_angle = laser_front.phi(0);
            // float max_angle = laser_front.phi(laser_front.size()-1);
            for (unsigned int i=0; i<(laser_front.size()); i++){
                float angle = (laser_front.phi(i) / LASER_ANGLE_RANGE_RATIO) + LASER_FRONT_POLAR_OFFSET;
                float dist = sqrt(laser_front.local(i).x*laser_front.local(i).x + laser_front.local(i).y*laser_front.local(i).y);
                processLaserScan(angle, dist, pg);
            }
        } catch(DBError err) {
            std::cerr << "Cannot read RTDB Object." << std::endl << err.what() << std::endl;
            try {
                    laser_front.RTDBSearch(laser_front_rtdbname.c_str());
                    laser_front.RTDBRead();
                    std::cout << "Successfully read " << laser_front_rtdbname << std::endl;
            } catch (DBError err) {
                    std::cerr << "Object not found." << std::endl;
                    std::cout << err.what();
            }
        }

        pg.update();

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

        // sleep(2.0);

    }
    return 0;
}
