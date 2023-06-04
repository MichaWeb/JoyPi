This is an c++ class driver for the DHT11 and DHT22 sensor.
For comunication PiGPIO libary is used, because WiringPi is out of date.

It supports 2 different returns, temerature as float and huminity as float.
DHT22 can be negative temperatures return.

But it returns only if checksum is correct.
