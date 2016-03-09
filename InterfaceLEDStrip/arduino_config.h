#ifndef ARDUINO_CONFIG_H
#define ARDUINO_CONFIG_H

#define NUMPIXELS           200
#define VOLTAGE_NUMPIXELS   8
#define PATTERN_DEPTH       4

#define STD_PRIO            10

#define PATTERN_SIZE sizeof(char) * NUMPIXELS * PATTERN_DEPTH
#define VOLTAGE_PATTERN_SIZE sizeof(char) * VOLTAGE_NUMPIXELS * PATTERN_DEPTH

#include <vector>

/*
typedef std::vector< char > color_t;
typedef std::vector< color_t > pattern_t;
typedef std::vector< color_t > voltage_pattern_t;
*/

#endif // ARDUINO_CONFIG_H

