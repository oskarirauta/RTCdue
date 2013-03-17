#ifndef _RTCdue_h_
#define _RTCdue_h_

// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

// Edited by Oskari Rauta. 
// i2c functionality adapted from DS1307 library of Henning Karlsen - no more wire.h needed
// Due now supported and calculation of 
// day of week now works. 3/16/2013

  
#include <Arduino.h>
  
// Simple general-purpose date/time class (no TZ / DST / leap second handling!)

#define DS1307_ADDR_R	209
#define DS1307_ADDR_W	208
#define SECONDS_PER_DAY 86400L
#define SECONDS_FROM_1970_TO_2000 946684800
  
  
class DateTime {
public:
    DateTime (uint32_t t =0);
    DateTime (uint16_t year, uint8_t month, uint8_t day,
                uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
    DateTime (const char* date, const char* time);
    uint16_t year() const       { return 2000 + yOff; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
    uint8_t dayOfWeek() const;

    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;   
    // 32-bit times as seconds since 1/1/1970
    uint32_t unixtime(void) const;

protected:
    uint8_t yOff, m, d, hh, mm, ss;
};

// RTC based on the DS1307 chip connected via I2C and the Wire library
class RTC_DS1307 {
public:
  RTC_DS1307(const uint8_t data_pin = PIN_WIRE_SDA, const uint8_t sclk_pin = PIN_WIRE_SCL);
  void begin(void);
  void adjust(const DateTime& dt);
  uint8_t isrunning(void);
  DateTime now(void);
private:
  uint8_t _scl_pin;
  uint8_t _sda_pin;
  uint8_t _burstArray[8];
  
  void _sendStart(byte addr);
  void _sendStop(void);
  void _sendAck(void);
  void _sendNack(void);
  void _waitForAck(void);
  uint8_t _readByte(void);
  void _writeByte(uint8_t value);
  void _burstRead(void);
  uint8_t _readRegister(uint8_t reg);
  void _writeRegister(uint8_t reg, uint8_t value);
  uint8_t _decode(uint8_t value);
  uint8_t _encode(uint8_t value);
};

// RTC using the internal millis() clock, has to be initialized before use
// NOTE: this clock won't be correct once the millis() timer rolls over (>49d?)
class RTC_Millis {
public:
    static void begin(const DateTime& dt) { adjust(dt); }
    static void adjust(const DateTime& dt);
    static DateTime now();

protected:
    static long offset;
};

#endif
        
        
