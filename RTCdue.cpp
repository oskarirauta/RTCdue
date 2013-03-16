// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

// Edited by SomeRandomGuy to implement support for the Arduino Due on 1/19/2013

// Edited more by Oskari Rauta. Due now supported and calculation of day of week now works. 3/16/2013

#include "RTCdue.h"

// utility code, some of this could be exposed in the DateTime API if needed

const uint8_t daysInMonth [] = { 31,28,31,30,31,30,31,31,30,31,30,31 }; //has to be const or compiler compaints

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += *daysInMonth + i - 1;
    if (m > 2 && y % 4 == 0)
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second

DateTime::DateTime (uint32_t t) {
  t -= SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0; ; ++yOff) {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m) {
        uint8_t daysPerMonth = *daysInMonth + m - 1;
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
}

static uint8_t conv2d(const char* p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

// A convenient constructor for using "the compiler's time":
//   DateTime now (__DATE__, __TIME__);
// NOTE: using PSTR would further reduce the RAM footprint
DateTime::DateTime (const char* date, const char* time) {
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    yOff = conv2d(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec 
    switch (date[0]) {
        case 'J': m = date[1] == 'a' ? 1 : m = date[2] == 'n' ? 6 : 7; break;
        case 'F': m = 2; break;
        case 'A': m = date[2] == 'r' ? 4 : 8; break;
        case 'M': m = date[2] == 'r' ? 3 : 5; break;
        case 'S': m = 9; break;
        case 'O': m = 10; break;
        case 'N': m = 11; break;
        case 'D': m = 12; break;
    }
    d = conv2d(date + 4);
    hh = conv2d(time);
    mm = conv2d(time + 3);
    ss = conv2d(time + 6);
}

uint8_t DateTime::dayOfWeek() const {    
     // CalendarSystem = 1 for Gregorian Calendar
  
  	 uint16_t year = yOff + 2000;
  	 uint8_t result, month = m;
  
     if ( month < 3)
     {
           month = month + 12;
           year -= 1;
     }
  	  
  	 result = (d + ( 2 * month ) + int(6 * ( month + 1 ) / 10) +
       		year + int(year / 4) - int(year / 100) +
       		int(year / 400) + 1 ) %7;
  
  	return result == 0 ? 7 : result;
}

uint32_t DateTime::unixtime(void) const {
  uint32_t t;
  uint16_t days = date2days(yOff, m, d);
  t = time2long(days, hh, mm, ss);
  t += SECONDS_FROM_1970_TO_2000;  // seconds from 1970 to 2000

  return t;
}

////////////////////////////////////////////////////////////////////////////////
// RTC_DS1307 implementation

RTC_DS1307::RTC_DS1307(const uint8_t data_pin, const uint8_t sclk_pin) {
  _sda_pin = data_pin;
  _scl_pin = sclk_pin;
  pinMode(_scl_pin, OUTPUT);
}

void RTC_DS1307::begin(void) {
  uint8_t _reg = _readRegister(0);
  _reg &= ~(1 << 7);
  _reg |= (0 << 7);
  _writeRegister(0, _reg);
}

void	RTC_DS1307::_sendStart(byte addr)
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_sda_pin, HIGH);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, LOW);
	shiftOut(_sda_pin, _scl_pin, MSBFIRST, addr);
}

void	RTC_DS1307::_sendStop(void)
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_sda_pin, HIGH);
	pinMode(_sda_pin, INPUT);
}

void	RTC_DS1307::_sendNack(void)
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_scl_pin, LOW);
	digitalWrite(_sda_pin, HIGH);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_scl_pin, LOW);
	pinMode(_sda_pin, INPUT);
}

void	RTC_DS1307::_sendAck(void)
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_scl_pin, LOW);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_scl_pin, LOW);
	pinMode(_sda_pin, INPUT);
}

void	RTC_DS1307::_waitForAck(void)
{
	pinMode(_sda_pin, INPUT);
	digitalWrite(_scl_pin, HIGH);
	while (_sda_pin==LOW) {}
	digitalWrite(_scl_pin, LOW);
}

uint8_t RTC_DS1307::_readByte(void)
{
	pinMode(_sda_pin, INPUT);

	uint8_t value = 0;
	uint8_t currentBit = 0;

	for (int i = 0; i < 8; ++i)
	{
		digitalWrite(_scl_pin, HIGH);
		currentBit = digitalRead(_sda_pin);
		value |= (currentBit << 7-i);
		delayMicroseconds(1);
		digitalWrite(_scl_pin, LOW);
	}
	return value;
}
void RTC_DS1307::_writeByte(uint8_t value)
{
	pinMode(_sda_pin, OUTPUT);
	shiftOut(_sda_pin, _scl_pin, MSBFIRST, value);
}

uint8_t RTC_DS1307::_readRegister(uint8_t reg)
{
	uint8_t	readValue=0;

	_sendStart(DS1307_ADDR_W);
	_waitForAck();
	_writeByte(reg);
	_waitForAck();
	_sendStop();
	_sendStart(DS1307_ADDR_R);
	_waitForAck();
	readValue = _readByte();
	_sendNack();
	_sendStop();
	return readValue;
}

void RTC_DS1307::_writeRegister(uint8_t reg, uint8_t value)
{
	_sendStart(DS1307_ADDR_W);
	_waitForAck();
	_writeByte(reg);
	_waitForAck();
	_writeByte(value);
	_waitForAck();
	_sendStop();
}

void RTC_DS1307::_burstRead(void)
{
	_sendStart(DS1307_ADDR_W);
	_waitForAck();
	_writeByte(0);
	_waitForAck();
	_sendStop();
	_sendStart(DS1307_ADDR_R);
	_waitForAck();

	for (int i=0; i<8; i++)
	{
		_burstArray[i] = _readByte();
		if (i<7)
			_sendAck();
		else
			_sendNack();
	}
	_sendStop();
}

uint8_t	RTC_DS1307::_decode(uint8_t value)
{
	uint8_t decoded = value & 127;
	decoded = (decoded & 15) + 10 * ((decoded & (15 << 4)) >> 4);
	return decoded;
}

uint8_t RTC_DS1307::_encode(uint8_t value)
{
	uint8_t encoded = ((value / 10) << 4) + (value % 10);
	return encoded;
}

uint8_t RTC_DS1307::isrunning(void) {
  return !(_readRegister(0)>>7);
}

void RTC_DS1307::adjust(const DateTime& dt) {
  _writeRegister(0, _encode(dt.second()));
  _writeRegister(1, _encode(dt.minute()));
  _writeRegister(2, _encode(dt.hour()));
  _writeRegister(4, _encode(dt.day()));
  _writeRegister(5, _encode(dt.month()));
  _writeRegister(6, _encode(dt.year()-2000));
}

DateTime RTC_DS1307::now(void) {
  _burstRead();
  uint8_t ss = _decode(_burstArray[0]);
  uint8_t mm = _decode(_burstArray[1]);
  uint8_t hh = _decode(_burstArray[2]);
  uint8_t d = _decode(_burstArray[4]);
  uint8_t m = _decode(_burstArray[5]);
  uint16_t y = _decode(_burstArray[6]) + 2000;
  return DateTime (y, m, d, hh, mm, ss);
}

////////////////////////////////////////////////////////////////////////////////
// RTC_Millis implementation

long RTC_Millis::offset = 0;

void RTC_Millis::adjust(const DateTime& dt) {
    offset = dt.unixtime() - millis() / 1000;
}

DateTime RTC_Millis::now() {
  return (uint32_t)(offset + millis() / 1000);
}

////////////////////////////////////////////////////////////////////////////////

        
        
        