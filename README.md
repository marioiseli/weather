# Arduino/Pi Weather station

The Arduino has a Grove BME820 barometer module connected on the I2C port, and a LCD display which also shows the usual values (temperature, pressure, humidity)
The Raspberry Pi running Raspbian reads the periodic reported values from the serial interface (parser written in C) and stores this as a JSON file. A simple Python Flask web application shows a Web UI with the current values and some periodicly refreshed graphs built using RRDTool.

See it in [action](http://msiseli.internet-box.ch/)
