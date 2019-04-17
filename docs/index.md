# Sensor-based Data Logging Using Microcontrollers

There are several ways to save data from a sensor attached to a microcontroller.  If you're connected to a personal computer, you can simply send the data from the controller to the personal computer serially, and save it to a file.  If you've got an SD card attached to the microcontroller, you can save the data to the SD card. Or, if you have access to the internet and a device that can connect to a server, you can save the data to a server. 

This site contains a number of data logging examples I've written for different workshops on the topic.

In these, you'll read a DHT11 temperature and humidity sensor and log data in three ways:

* Serial transmission to a personal computer, and serial capture to a file.
* Saving data to an SD card mounted on the Arduino
* HTTP upload to pachube.com via an Ethernet shield or Ethernet Arduino.

I presented[http://www.tigoe.net/pcomp/code/?p=1043](an introductory workshop) [http://cybersciencesummit.org/2012/02/summit-coverage-on-gridcast/](Citizen Science Cybersummit) in  London in 2012. 

