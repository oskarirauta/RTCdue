RTCdue
======

DS1703 library for DUE that doesn't need wire.h

Combination of Adafruit's RTClib and Henning Karlsen's DS1703 RTC library ( http://www.henningkarlsen.com/electronics/library.php?id=34 ).

Henning's library grabbed stored day of week from DS1703 and RTClib had a calculation that was wrong and didn't take leap years into account. I've added a working calculation. On usage; check provided examples and Adafruit's documentation.

Warning: if you are using Aqualed's tft/screw shield with RTC and want to utilize display brightness by powering display's
led from pin10, use analogWrite(10, 250); as setting it to higher is equillavent with digitalWrite(10, HIGH); and for the moment reason is unknown, but i2c/ds1703 becomes in-accessible.

Credits from great code go to Adafruit and Henning Karlsen. Check possible licenses.
