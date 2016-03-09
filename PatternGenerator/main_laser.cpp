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

//#include <signal.h> // necessary?

#define RTDB_SEARCH_WAIT_TIMEOUT 1.0
#define MIN_VOLTAGE 45.0
#define MAX_VOLTAGE 52.0


// Constants for Laser Positions relative to LED strip
// LED strip index 0 is on back right side of the robot
// direction of increasing index is CCW
#define LASER_MAX_RANGE             1.5f    // in meter, color intensity of the LED strip starts with 0 at this range
#define LED_MAX_INTENSITY           5.0   // maximum color intensity of the LED strip (range: 0 ... 255)

// pixel mapping
#define LASER_REAR_PIXEL_OFFSET     0
#define LASER_FRONT_PIXEL_OFFSET    91      // pixel index from 0 to 181 (182 pixels on the strip)
#define NUM_CORNER_PIXELS           2       // number of pixels on each edge of the strip, assigned to the corner (results in the double number of leds asigned to one corner)
#define NUM_SHORT_EDGE_PIXELS       36
#define NUM_LONG_EDGE_PIXELS        55
#define PIXEL_PER_METER             60
#define MIN_DIST                    0.05    // minimum distance of an object to the robot to be mapped on the led strip
                                            // problem: laser detects objects on the robot (connector on front an back side)

using namespace std;

#include <kogmo_rtdb.hxx>
using namespace KogniMobil;


int calcIntensity(float y)
{
    float lin_intensity = (-LED_MAX_INTENSITY / LASER_MAX_RANGE) * y + LED_MAX_INTENSITY;

    if (lin_intensity > LED_MAX_INTENSITY) lin_intensity = LED_MAX_INTENSITY;
    if (lin_intensity < 0) lin_intensity = 0;

    return lin_intensity;
}

void cornerMapping(int index, char intensity, PatternGenerator &pg)
{
    int set_index;
    for (int i=-NUM_CORNER_PIXELS; i < NUM_CORNER_PIXELS; i++) {
        //cout << "set pixel:" << (pixel_offset + i) << endl;
        set_index = index + i;
        if ((set_index) < 0) {
            set_index += pg.getMaxPixels();
        }

        pg.setColor(0x00, 0x00, intensity);
        pg.setPixel(set_index);
    }
}

float calcDistFromCorner(float x, float y, float cornerDist)
{
    float cx = abs(x) - cornerDist;
    return sqrt(cx*cx + y*y);
}

// returns true if calculated pixel is on a corner
// (prevents to set pixels, which are reserved for the corner mapping algorithm)
// otherwise, pixels in corners would flicker
bool IsCornerIndex(int index)
{
    bool retVal = false;
    if(
            (index < (LASER_REAR_PIXEL_OFFSET+NUM_CORNER_PIXELS) && index >= (LASER_REAR_PIXEL_OFFSET-NUM_CORNER_PIXELS)) ||
            (index < (LASER_REAR_PIXEL_OFFSET-NUM_SHORT_EDGE_PIXELS+NUM_CORNER_PIXELS) && index >= (LASER_REAR_PIXEL_OFFSET-NUM_SHORT_EDGE_PIXELS-NUM_CORNER_PIXELS)) ||
            (index < (LASER_REAR_PIXEL_OFFSET+NUM_LONG_EDGE_PIXELS+NUM_CORNER_PIXELS) && index >= (LASER_REAR_PIXEL_OFFSET+NUM_LONG_EDGE_PIXELS-NUM_CORNER_PIXELS)) ||

            (index < (LASER_FRONT_PIXEL_OFFSET+NUM_CORNER_PIXELS) && index >= (LASER_FRONT_PIXEL_OFFSET-NUM_CORNER_PIXELS)) ||
            (index < (LASER_FRONT_PIXEL_OFFSET-NUM_SHORT_EDGE_PIXELS+NUM_CORNER_PIXELS) && index >= (LASER_FRONT_PIXEL_OFFSET-NUM_SHORT_EDGE_PIXELS-NUM_CORNER_PIXELS)) ||
            (index < (LASER_FRONT_PIXEL_OFFSET+NUM_LONG_EDGE_PIXELS+NUM_CORNER_PIXELS) && index >= (LASER_FRONT_PIXEL_OFFSET+NUM_LONG_EDGE_PIXELS-NUM_CORNER_PIXELS))
            )
        retVal =true;

    return retVal;
}


// assign each laser ray to a LED on the strip
void processLaserScan(float angle, float dist, PatternGenerator &pg, int pixel_offset, float &min_laser_corner_dist, float &min_short_corner_dist)
{
    int set_index, pixel_index;
    float x, y, corner_dist;

    // -45 ... 45 : corner pixel mapping (map 4 leds on each corner of the robot)
    if (angle >= (-0.25*M_PI) && angle <= (0.25*M_PI)) {

        if (dist < min_laser_corner_dist) {

            min_laser_corner_dist = dist;
            cornerMapping(pixel_offset, calcIntensity(dist), pg);
        }
    }
    else {
        // -135 ... -45 : short edge pixel mapping
        if (angle < (-0.25*M_PI)) {
            y = dist * sin(angle + 0.75*M_PI);
            x = -(dist * cos(angle + 0.75*M_PI));

            if (y > MIN_DIST) {
                // if coordinates in corner, map it
                if (abs(x) > pg.getShortEdgeLength()) {

                    // calc object distance to corner
                    corner_dist = calcDistFromCorner(x, y, pg.getShortEdgeLength());

                    // light up corner if object distance to corner is below threshold
                    if (corner_dist < min_short_corner_dist) {
                        //cout << "corner map, angle: " << (angle/M_PI*180) << " dist: " << dist << " corner_dist: " << corner_dist << endl;
                        min_short_corner_dist = corner_dist;

                        set_index = pixel_offset - NUM_SHORT_EDGE_PIXELS;
                        if (set_index < 0) {
                            set_index += pg.getMaxPixels();
                        }
                        cornerMapping(set_index, calcIntensity(corner_dist), pg);
                    }
                }
                // standard mapping, if object is not too close to the robot
                else {
                    pg.setColor(0x00, 0x00, calcIntensity(y));

                    pixel_index = (int)(x * pg.getPixelPerMeter());
                    set_index = pixel_index + pixel_offset;

                    if(!IsCornerIndex(set_index)) {
                        if (set_index < 0) {
                            set_index += pg.getMaxPixels();
                        }
                        pg.setPixel(set_index);
                    }
                }
            }
        }
        // 45 ... 135 : long edge pixel mapping
        else if (angle > (0.25*M_PI)) {
            y = dist * cos(angle - 0.25*M_PI);
            x = (dist * sin(angle - 0.25*M_PI));

            if (y > MIN_DIST) {
                // if coordinates in corner, map it
                // INFO: this code was used, when both laser mapped their object distances to the corner, which is not equipped with a laser
                // mapping only through one laser, which is closer to the specific corner was purposeful
                /*
                if (abs(x) > pg.getLongEdgeLength()) {

                    // calc object distance from corner
                    corner_dist = calcDistFromCorner(x, y, pg.getLongEdgeLength());

                    // light up corner if object distance to corner is below threshold
                    if (corner_dist < LASER_MAX_RANGE) {
                        set_index = pixel_offset + NUM_LONG_EDGE_PIXELS;    // TODO check sign for other laser
                        if (set_index < 0) {
                            set_index += pg.getMaxPixels();
                        }
                        cornerMapping(set_index, pg);
                    }
                }
                */
                // standard mapping, if object is not too close to the robot
                if (abs(x) < pg.getLongEdgeLength() && y > MIN_DIST) {
                    pg.setColor(0x00, 0x00, calcIntensity(y));

                    pixel_index = (int)(x * pg.getPixelPerMeter());
                    set_index = pixel_index + pixel_offset;

                    if(!IsCornerIndex(set_index)) {
                        if (set_index < 0) {
                            set_index += pg.getMaxPixels();
                        }
                        pg.setPixel(set_index);
                    }
                }
            }
        }
    }
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

    int display_counter = 0;

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

    cout << "Entering loop and publishing Laser state" << endl;

    while (1) {

        display_counter++;

        // clear all pixels from laser data
        // argument is priority of clearing a pixel (determines how fast pixels are switched off);
        pg.clear(8);

        // REAR LASER
        try {
            laser_rear.RTDBRead();
            // float min_angle = laser_rear.phi(0);
            // float max_angle = laser_rear.phi(laser_rear.size()-1);
            float min_laser_corner_dist = 9999.0;
            float min_short_corner_dist = 9999.0;

            for (unsigned int i=0; i<(laser_rear.size()); i++){
                float angle = laser_rear.phi(i);
                float dist = laser_rear.raw(i) / 1000.0; // from mm to m
                processLaserScan(angle, dist, pg, LASER_REAR_PIXEL_OFFSET, min_laser_corner_dist, min_short_corner_dist);
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

        //cout << "---------------" << endl;

        // FRONT LASER
        try {
            laser_front.RTDBRead();
            // float min_angle = laser_front.phi(0);
            // float max_angle = laser_front.phi(laser_front.size()-1);
            float min_laser_corner_dist = 9999.0;
            float min_short_corner_dist = 9999.0;

            for (unsigned int i=10; i<(laser_front.size()); i++){    // TODO check start index, there was a object detection on ray 0 for front laser scanner
                float angle = laser_front.phi(i);
                float dist = laser_front.raw(i) / 1000.0; // from mm to m
                processLaserScan(angle, dist, pg, LASER_FRONT_PIXEL_OFFSET, min_laser_corner_dist, min_short_corner_dist);
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

        pg.updateStrip();

        //cout << "main: Write to RTDB ...";
        try {
            ard_rtdb_obj.RTDBWrite();
            //cout << "done" << endl;
            DBC.CycleDone();
            // if (display_counter%50000 == 0) cout << ".\n";
        }
        catch(KogniMobil::DBError err)
        {
            cerr << "main: Error when writing to RTDB object: " << err.what() <<  endl;
            cerr << "Is arduino_led_interface running?" << endl;
        }

        //sleep(1.0);

    }
    return 0;
}
