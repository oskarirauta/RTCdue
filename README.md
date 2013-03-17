RTCdue
======

DS1307 library for DUE that doesn't need wire.h. Should work on others as well, just make sure you initialize with sda and scl pins if you cannot get it to work..

Combination of Adafruit's RTClib and Henning Karlsen's DS1307 RTC library ( http://www.henningkarlsen.com/electronics/library.php?id=34 ).

Henning's library grabbed stored day of week from DS1307 and RTClib had a calculation that was wrong and didn't take leap years into account. I've added a working calculation. On usage; check provided examples and Adafruit's documentation.

Warning: if you are using Aqualed's tft/screw shield with RTC and want to utilize display brightness by powering display's
led from pin10, use analogWrite(10, 250); as setting it to higher is equillavent with digitalWrite(10, HIGH); and for the moment reason is unknown, but i2c/ds1307 becomes in-accessible.
This issue can possibly be ignored when using external power source - since usb only provides 500mA and external can provide more - but I haven't tested this.

Credits from great code go to Adafruit and Henning Karlsen.

When using code of Henning Karlsen, it is necessary to release the code under similar license as his, therefore this code is 
licensed as: GNU Lesser General Public License v2.1

You can read the license at http://www.gnu.org/licenses/lgpl-2.1.html
