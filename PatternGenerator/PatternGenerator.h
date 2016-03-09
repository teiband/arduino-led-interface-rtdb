/*
 * PatternGenerator.h
 *
 *  Created on: 29 Oct 2015
 *      Author: Thomas Eiband
 *
 *      thomas.eiband@tum.de
 */

#ifndef MULTIPLUS_H_
#define MULTIPLUS_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../InterfaceLEDStrip/arduino_config.h"
#include <iostream>

// Include for RTDB object
#include <ArduinoData.h>

// get common arduino constants from xml file
// uint32_t pattern[NUMPIXELS][3];

using namespace std;

class PatternGenerator
{
private:

    char color[PATTERN_DEPTH];
    char pattern[NUMPIXELS][PATTERN_DEPTH];
    char voltage_pattern[NUMPIXELS][PATTERN_DEPTH];
    bool emergency_state;

    KogniMobil::ArduinoData &rtdb_obj;

    void copy2pattern(int i);
    void checkRange(int i);

    int numpixels;

public:

    PatternGenerator(KogniMobil::ArduinoData &rtdb_obj);

    void updateAll();
    void updateStrip();
    void updateVoltageStrip();

    void setRawPattern(char *pattern);
    void setFillPattern(unsigned int spacing = 0);

    void setFillArea(unsigned int start, unsigned int end, unsigned int spacing = 0);
    
    void shiftPattern(int digits = 1);

    virtual ~PatternGenerator();

    void setBrightness(float brightness);
    void setPixel(unsigned int pixel);
    void setFadingArea(unsigned int start, int range, bool single_sided);
    void clear(unsigned char prio = 0);
    void setColor(char red, char green, char blue, unsigned char prio = STD_PRIO);
    void setColor(char color, unsigned char prio = STD_PRIO);
    void setColor(uint32_t color_value, unsigned char prio = STD_PRIO);
    void dimLight(char *buf, float factor);
    void debugPattern(int flag) const;
    void debugLEDs(int flag);
    void setBattVoltage(float voltage, float min_voltage, float max_voltage);
    void processBattVoltage(float level);
    void setBattVoltagePattern(char voltage_pattern[][PATTERN_DEPTH]);
    void setBattVoltageFillPattern(unsigned int spacing = 0);

    // getter functions
    int getMaxPixels();
    int getPixelPerMeter();
    float getShortEdgeLength();
    float getLongEdgeLength();

    // functions for polar coordinate assignment of LEDs
    float index2polar(int index);
    int polar2index(float rad);

};

#endif /* MULTIPLUS_H_ */
