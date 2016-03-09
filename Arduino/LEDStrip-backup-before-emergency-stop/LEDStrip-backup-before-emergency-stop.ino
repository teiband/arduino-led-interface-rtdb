// original version (with rtdb)

// for Adafruit DotStar:
// ############################################
#include <Adafruit_DotStar.h>
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

// Here's how to control the LEDs from any two pins:
#define DATAPIN       11
#define CLOCKPIN      13
// #############################################

// for Adafruit NeoPixel:
// #############################################
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIXEL_DATA_PIN 6
#define E_STOP_PIN        5

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel voltage_strip = Adafruit_NeoPixel(60, NEOPIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// #############################################


#define TOTAL_BYTES       62//32  // Full message size
#define TOTAL_DATA        60//30  // Payload size
#define NUMPIXELS         200
#define LEDS_PER_PACKAGE  20  // TOTAL_DATA/3
// be careful: following statement did not work:
// #define TOTAL_LED_PACKETS NUMPIXELS/LEDS_PER_PACKAGE
// preprocessor error!
#define TOTAL_LED_PACKETS 10

#define VOLTAGE_PACKAGE_CNT TOTAL_LED_PACKETS

#define VOLTAGE_NUMPIXELS 8

// 115200
#define BAUDRATE 1000000L

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_RGB);
  // TODO: above change color set to right value DOTSTAR_RGB ...


/* Protokoll:
 *  0.  header
 *  1.  ...
 *  60. ... data
 *  61. checksum
 */

 // Colors: BLUE RED GREEN

void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}

void setup()
{
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  voltage_strip.begin(); // Initialize pins for output
  voltage_strip.show();  // Turn all LEDs off ASAP

  pinMode(E_STOP_PIN, INPUT); 

  // TODO check timeout
  Serial.setTimeout(1);  // timeout in ms
  
  Serial.begin(BAUDRATE);
}

char buf[TOTAL_BYTES];  // contains only data, not full package information

uint8_t calcChecksum(char* frame, uint8_t frameLength)
{
    uint8_t checksum = 0;
    for(uint8_t i=0; i < frameLength; i++) {
        checksum += frame[i];
    }
    return checksum;
}

void updateVoltageStrip(char* buf)
{
  for (int i=0; i < VOLTAGE_NUMPIXELS; i++) {
    voltage_strip.setPixelColor(i, buf[i*3], buf[i*3+1], buf[i*3+2]);
  }
}

int freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void updateStrip(int packet_counter, char* buf)
{
  for (int i=0; i < LEDS_PER_PACKAGE; i++) {
    // uint32_t color = (uint32_t)buf[i*3] | (uint32_t)buf[i*3+1] << 8 | (uint32_t)buf[i*3+2] << 16; 
    strip.setPixelColor(i + (packet_counter*LEDS_PER_PACKAGE), buf[i*3], buf[i*3+1], buf[i*3+2]);  
  } 
}

void shutOffStrip()
{
  strip.clear();
  strip.show();
  voltage_strip.clear();
  voltage_strip.show();
}

char packet_counter = 0;

void debugLED(int n)
{
  strip.setPixelColor(n, 0x0F0F00);
  strip.show();
}

uint32_t timeout_cnt = 0;

void loop(){

  int checksum = 0;
  
  if ( Serial.available() >= TOTAL_BYTES)
  {
    char first_byte = Serial.read();
    
    if(first_byte == packet_counter)
    {    
      uint8_t n = Serial.readBytes(buf, TOTAL_BYTES - 1);
      
      // received a full package?
      if (n == (TOTAL_BYTES - 1))
      {    
        checksum = calcChecksum(buf, TOTAL_BYTES-1);
        if (checksum == 0) {
          if (packet_counter == VOLTAGE_PACKAGE_CNT) {
            updateVoltageStrip(buf);
            voltage_strip.show();
          }
          else {
            updateStrip(packet_counter, buf);
          }
          // increase packet_counter after full package received and checksum is right!
          packet_counter++;
        }
        // received the last package?
        if (packet_counter == (TOTAL_LED_PACKETS+1)) {
          packet_counter = 0;
          strip.show();
          timeout_cnt = 0;  // reset timeout counter after successful transmission
        }
        // after receiving full package, answer to pc:
        serialFlush();  // empty serial input buffer (data is corrupted after this received package!)
        // TODO emergency stop: this behaviour requires permanent communication with Arduino to receive the emergency stop state
        // eg permanently writing zero color information with zero priority to keep the communication running!
        if ( digitalRead(E_STOP_PIN) )
          Serial.write('E');
        else
          Serial.write('R');
        Serial.write(packet_counter); // send new package_counter to PC
      }
    }
    // received the wrong package:
    else {
      serialFlush();  // empty serial input buffer, if header is wrong
      Serial.write('W');
      Serial.write(packet_counter); // send actual package_counter to PC
    }
  }
  // not all bytes of a package received yet:
  else {
    timeout_cnt++;
    // Wait for transmission to come in
    delayMicroseconds(5000);
    // send arduino expects new transmission: package zero:
    if (packet_counter == 0 && !Serial.available()) {
      Serial.write('N');
      Serial.write(packet_counter);    
    }
    else {    
      // shut all LEDs off after ca. 10 seconds
      if ((++timeout_cnt) >= 500) {
        shutOffStrip();
        timeout_cnt = 0;
      }
    }   
  } 
}
