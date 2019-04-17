# Sensor-based Data Logging Using Microcontrollers

There are several ways to save data from a sensor attached to a microcontroller.  If you're connected to a personal computer, you can simply send the data from the controller to the personal computer serially, and save it to a file.  If you've got an SD card attached to the microcontroller, you can save the data to the SD card. Or, if you have access to the internet and a device that can connect to a server, you can save the data to a server. 

This site contains a number of data logging examples I've written for different workshops on the topic over the years. The most recent examples reflect my most up-to-date practices, of course.    

## 2019: MKR Series Examples
In 2019 I started writing examples for the MKR series of Arduinos, which are based on the SAMD21 ARM M0+ processor. The built-in realtime clock on the MKR boards, along with other features, make them my current favorite for datalogging.

## 2015: Brown Institute for Media Innovation Workshop
I presented [a workshop](brown-workshop.md)
 at the [Brown Institute for Media Innovation](http://brown.columbia.edu/) 
 at Columbia University. [These examples](brown-workshop.md) are from that workshop. In this workshop, you'll find examples of how to use a Feather M0 Adalogger from Adafruit.com to read data from sensors and save to an SD card.

## 2012: Citizen Science Cybersummit Workshop
I presented [an introductory workshop](http://www.tigoe.net/pcomp/code/?p=1043) at the [Citizen Science Cybersummit](http://cybersciencesummit.org/2012/02/summit-coverage-on-gridcast/) in London in 2012. [These examples](https://github.com/tigoe/DataloggingExamples/tree/master/Citizen_Cyberscience_summit_2012) are from that workshop.  In the examples from that workshop, you'll find examples on how to read a DHT11 temperature and humidity sensor and log data in three ways:

* Serial transmission to a personal computer, and serial capture to a file.
* Saving data to an SD card mounted on the Arduino
* HTTP upload to pachube.com  (later [xively.com](http://xively.com/) and now part of the Google Cloud platform) via an Ethernet shield or Ethernet Arduino. _(caveat: the Ethernet Arduino is no longer in production, and I haven't tested these examples since pachube.com became xively.com)_

