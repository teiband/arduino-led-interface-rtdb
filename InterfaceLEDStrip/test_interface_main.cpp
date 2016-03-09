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

using namespace std;

#include <kogmo_rtdb.hxx>
using namespace KogniMobil;


int main()
{

    const char *deviceName = "/dev/ttyACM0";
    arduino ard(deviceName);

    char r = 0;
    char g = 0;
    char b = 0;

    char max_val = 10;

    while(1) {
        ard.setColor((r++), (g++), (b++));
        ard.fillColor();

        ard.write();

        sleep(0.01);

        if (r > max_val) r = 0;
        if (g > max_val) g = 0;
        if (b > max_val) b = 0;
    }

}



