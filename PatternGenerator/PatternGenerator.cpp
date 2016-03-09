/*
 * PatternGenerator.cpp
 *
 *  Created on: 29 Oct 2015
 *      Author: Thomas Eiband
 *
 *      thomas.eiband@tum.de
 */

#include "PatternGenerator.h"

using namespace std;


// TODO References cannot be reassignet, instead use a pointer!

PatternGenerator::PatternGenerator(KogniMobil::ArduinoData &rtdb_obj) :
    rtdb_obj(rtdb_obj)
{
    /*
    color           =   rtdb_obj.subobj_p->color;
    pattern         =   rtdb_obj.subobj_p->pattern;
    voltage_pattern =   rtdb_obj.subobj_p->voltage_pattern;
    */
    numpixels = 190;
}

void PatternGenerator::setRawPattern( char *pattern)
{
    memcpy(this->pattern, pattern, PATTERN_SIZE);
}

void PatternGenerator::setFillPattern(unsigned int spacing /* = 0 */)
{
    for (int i=0; i < NUMPIXELS; i++) {
        pattern[i][0] = color[0];
        pattern[i][1] = color[1];
        pattern[i][2] = color[2];
        pattern[i][3] = color[3];
    // TODO add spacing functionality
    }
}

void PatternGenerator::setBattVoltageFillPattern(unsigned int spacing /* = 0 */)
{
    for (int i=0; i < NUMPIXELS; i++) {
        voltage_pattern[i][0] = color[0];
        voltage_pattern[i][1] = color[1];
        voltage_pattern[i][2] = color[2];
        voltage_pattern[i][3] = color[3];
    // TODO add spacing functionality
    }
}

int PatternGenerator::getMaxPixels()
{
    return 182;
}

int PatternGenerator::getPixelPerMeter()
{
    return 60;
}

float PatternGenerator::getShortEdgeLength()
{
    return 36 / (float)getPixelPerMeter();
}

float PatternGenerator::getLongEdgeLength()
{
    return 55 / (float)getPixelPerMeter();
}

float PatternGenerator::index2polar(int index)
{
    double rad = index / ((float)NUMPIXELS) * 2 * M_PI;
    if (rad > (2*M_PI)) rad = rad - (2*M_PI);
    if (rad < 0) rad = rad + (2*M_PI);
    return rad;
}

int PatternGenerator::polar2index(float rad)
{
    if (rad > (2*M_PI)) rad = rad - (2*M_PI);
    if (rad < 0) rad = rad + (2*M_PI);
	// TODO add constant to substract from NUMPIXELS for real strip length
    return rad / (2*M_PI) * (NUMPIXELS);
}

void PatternGenerator::setColor(char red, char green, char blue, unsigned char prio /* = STD_PRIO */)
{
    // color order is defined in arduino (HW) constant DOTSTAR_RGB
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = prio;
}

void PatternGenerator::setColor(char color, unsigned char prio /* = STD_PRIO */)
{
    switch (color) {
        case 'r':
        case 'R':
            setColor(0x10, 0x00, 0x00, prio);
            break;
        case 'g':
        case 'G':
            setColor(0x00, 0x10, 0x00, prio);
            break;
        case 'b':
        case 'B':
            setColor(0x00, 0x00, 0x10, prio);
            break;
        case 'w':
        case 'W':
            setColor(0x10, 0x10, 0x10, prio);
            break;
    }
}

void PatternGenerator::setColor(uint32_t color_value, unsigned char prio /* = STD_PRIO */)
{
    // TODO check color order, compare with color generation part to shift in right way
    uint32_t bit_mask = 0x000011;
    color[0] = (char)(color_value & bit_mask);
    color[1] = (char)((color_value >> 8) & bit_mask);
    color[2] = (char)((color_value >> 16) & bit_mask);
    color[3] = prio;
}

void PatternGenerator::debugPattern(int flag) const
{
    cout << "debug pattern:" << endl;

    for (int i=0; i < NUMPIXELS; i++) {
        if (flag) {
            cout << (int)pattern[i][0] << ", " << pattern[i][1] << ", " << pattern[i][2] << ", " << pattern[i][3] << endl;
        }
        else {
            cout << (pattern[i][0] > 0 ? 'X' : '0');
            cout << (pattern[i][1] > 0 ? 'X' : '0');
            cout << (pattern[i][2] > 0 ? 'X' : '0');
            cout << (pattern[i][3] > 0 ? 'X' : '0');
        }

    }
    cout << endl;
}


// pass factor 0 ... 1 to dim light color pattern
void PatternGenerator::dimLight(char* buf, float factor) {
    for (int i=0; i < NUMPIXELS; i++) {
        for (int j=0; j < 3; j++) {
            pattern[i][j] = (char)(buf[i] * factor);
        }
    }
}

void PatternGenerator::setFillArea(unsigned int start, unsigned int end, unsigned int spacing /* = 0 */)
{
    checkRange(start);
    checkRange(end);

    for (int i=start; i < end; i += spacing) {
        copy2pattern(i);
    }
}

void PatternGenerator::updateAll()
{
    updateStrip();
    updateVoltageStrip();
}

void PatternGenerator::updateStrip()
{
    memcpy(this->rtdb_obj.subobj_p->pattern, this->pattern, PATTERN_SIZE);
}

void PatternGenerator::updateVoltageStrip()
{
    memcpy(this->rtdb_obj.subobj_p->voltage_pattern, this->voltage_pattern, VOLTAGE_PATTERN_SIZE);
}

void PatternGenerator::copy2pattern(int i)
{
    // TODO faster with iterators (begin ... end)
    for (int j=0; j < 4; j++)
        pattern[i][j] = color[j];
}

void PatternGenerator::setBrightness(float brightness)
{
    for (int i=0; i < 3; i++)
        color[i] *= brightness;
}

void PatternGenerator::setPixel(unsigned int pixel)
{
    checkRange(pixel);
    copy2pattern(pixel);
}

void PatternGenerator::checkRange(int i)
{
    // TODO also throw name of calling function with __func__
    if (i < 0 || i >= NUMPIXELS) {
        cerr << "Pixel index out of range. Range is from 0 to " << NUMPIXELS << endl;
        exit(EXIT_FAILURE);
    }
}

void PatternGenerator::setFadingArea(unsigned int start, int range , bool single_sided /* = false */)
{
    checkRange(start);
    checkRange(start+range);
    checkRange(start-range);

    // set start pixel color
    setPixel(start);
  
    if (single_sided = false) {
    range = abs(range);
    // create color transition:
    for (int i = range; i > 0; i--) {
        setBrightness(i / range);
        setPixel(start++ );
        setPixel(start--);
    }
    }
    else {
    // create color transition:
    for (int i = abs(range); i > 0; i--) {
        setBrightness(i / range);
        if (range > 0)
        setPixel(start++);
        else
        setPixel(start--);
    }
  }

}

void PatternGenerator::shiftPattern(int digits /* = 1 */)
{
    checkRange(digits);


  if (abs(digits) >= NUMPIXELS) {
    cerr << "shift value too big: digits = " << digits << ", maximum is NUMPIXELS = " << NUMPIXELS << endl;
    exit(EXIT_FAILURE);
  }
  
  if (digits > 0) {
    for (int i=0; i < (NUMPIXELS); i++) {
      // check if index after shifting is bigger than NUMPIXELS, then beginn from 0 again
      // TODO
      // pattern[i] = pattern[((i + digits) >= NUMPIXELS ? (NUMPIXELS - 1 - i) : (i + digits))];
    }
  }
  if (digits < 0) {
    for (int i=0; i < (NUMPIXELS); i++) {
      // check if index after shifting is bigger than NUMPIXELS, then beginn from 0 again
      // TODO
      //pattern[i] = pattern[((i + digits) < 0 ? (NUMPIXELS - i) : (i + digits))];
    }
  }
  
}

void PatternGenerator::setBattVoltage(float voltage, float min_voltage, float max_voltage)
{
    // calc level from input data
    processBattVoltage((max_voltage - voltage) / (max_voltage - min_voltage));
}

void PatternGenerator::processBattVoltage(float level)
{
    if (level < 0) level = 0;
    if (level > 1) level = 1;

    char num_pixels = level * VOLTAGE_NUMPIXELS;

    for (int i=0; i < num_pixels; i++) {
        voltage_pattern[i][0] = 0x00;
        voltage_pattern[i][1]= 0x10;
        voltage_pattern[i][2] = 0x00;
        voltage_pattern[i][3] = STD_PRIO;
    }
    for (int i=num_pixels; i < 8; i++) {
        voltage_pattern[i][0] = 0x10;
        voltage_pattern[i][1]= 0x00;
        voltage_pattern[i][2] = 0x00;
        voltage_pattern[i][3] = STD_PRIO;
    }
}

void PatternGenerator::setBattVoltagePattern(char voltage_pattern[][PATTERN_DEPTH])
{
    memcpy(this->voltage_pattern, voltage_pattern, VOLTAGE_PATTERN_SIZE);
}

void PatternGenerator::clear(unsigned char prio /* = STD_PRIO */)
{
    setColor(0,0,0,prio);
    for (int i=0; i < NUMPIXELS; i++) {
        setPixel(i);
    }
}

PatternGenerator::~PatternGenerator()
{
  
}


