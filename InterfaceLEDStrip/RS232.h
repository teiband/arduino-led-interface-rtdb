/*
 * RS232.h
 *
 *  Created on: 21 Apr 2015
 *      Author: Thomas Eiband
 *
 *      thomas.eiband@tum.de
 */

/** Serial connection handling and read/write.
 *
 */

#ifndef RS232_H
#define RS232_H

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>


class RS232
{
private:
    /// file descriptor for the serial port
    int fd;
    /// device name of the serial port in linux, e.g. "/dev/multiplus"
    const char* deviceName;
    int baudrate = B1000000;    // previously B9600
    /**
     * @brief Debug function to display data frames as hex-values. The length information of the buffer is taken from the buffer itself
     * @param buf pointer to a char buffer
     */
    void debugHEX(char *buf) const;
public:
    /** Constructor
     * @param deviceName pointer to char array with device name of the serial port
     */
    RS232(const char* deviceName);

    /** Set the interface attributes after opening the connection
     * @param speed The baudrate of the interface, use definitions from <termios.h>. E.g. B2400 = 2400baud
     * @param parity Parity of the interface, 0 = false, 1 = true
     */
    int set_interface_attr (int speed) const;
    // u_int8_t set_blocking (u_int16_t number, int time);

    /** Opens the serial port
     * @return 0 for success, 1 for opening error
     */
    int rs232_open(void);
    /** Closes the serial port
     * @return 0 for success, 1 for closing error
     */
    int rs232_close(void) const;
    /**
     * @brief Writes to serial port
     * @param msg pointer to char array, which includes data frame
     * @param size framesize
     * @return no. of bytes written
     */
    int rs232_write(const char *msg, int size) const;
    /**
     * @brief rs232_read
     * @param buffer pointer to char array, where frame will be written
     * @param size framesize
     * @param time Minimum response time in miliseconds of the serial interface for a data frame
     * @return no. of bytes read
     */
    int rs232_read(char* buffer, unsigned int size, unsigned int time) const;

    /// Desctructor of RS232 class, closes the serial connection properly.
    virtual ~RS232();
};

#endif // RS232_H
