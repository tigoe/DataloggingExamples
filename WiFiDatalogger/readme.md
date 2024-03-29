# Datalogging to a Server via WiFi

The examples described here log data to a server via WiFi using a network-enabled microcontroller as a datalogging client. These examples should all work on the network-enabled MKR boards, along with the Nano 33 IoT and other SAMD-based boards which have network capability. They can likely work on Espressif-based boards (ESP32, ESP8266) with some modification.

This collection includes an [ArduinoHttpClient](https://www.arduino.cc/reference/en/libraries/arduinohttpclient/) example along with a [node.js](https://nodejs.org/) server script. There are also instructions for how to log data to a Google Sheets spreadsheet using [Google Apps script](https://developers.google.com/apps-script/reference/spreadsheet/spreadsheet-app), thanks to Arnab Chakravarty. 

The scripts:
* [ArduinoHttpClient logger](datalogger-circuit)
* [node.js server](node-datalogging-server/)
* [Google Apps script](google-sheets-datalogger/)

To make this work, you'll need:

* To be comfortable programming Arduinos
* A WiFi-connected Arduino like the Nano 33 IoT, MKR 1010, or MKR1000. 
* An input sensor (pushbutton or knob is fine)
* To be comfortable with programming in JavaScript in some [node.js](https://nodejs.org)
* a free [Glitch.com](https://www.glitch.com) account (for some examples)

Background material you might want to review beforehand:

* [Wifinina_startup](https://vimeo.com/showcase/6916443/video/400951453)
* [WiFi_status](https://vimeo.com/showcase/6916443/video/401078236)
* Read [A Gentle Introduction to HTTP](https://itp.nyu.edu/networks/explanations/a-gentle-introduction-to-http/) by Maria del pilar Gomez Ruiz to get an idea of how web clients and servers talk to each other. 

For other WiFi  and ArduinoHttpClient examples, see [this repository](https://tigoe.github.io/Wifi101_examples/). For other Node examples, see [this repository](https://tigoe.github.io/NodeExamples/).

## Where Are You Logging the Data?
The Arduino client is set up to log the data by default to a Google Apps Script, as explained [here](google-sheets-datalogger/). If you want to change it to send to a node.js script like the one shown [here](node-datalogging-server), then change the `server` and `route` variables to match the address of your server and the API route for posting. If you're running the node server via HTTP rather than via HTTPS, then change the `WiFiSSLClient` declaration to `WiFiClient` and change the `port` variable to the port number on which your node script is listening.

### Minimum Parameters To Log
When you're setting up a microcontroller to be a sensor datalogging client, there are a few things to consider: 
* What is the sensor data you're gathering?
* When was each sensor reading taken?
* Where was it taken?
* Who gathered it?

#### What Data Are You Gathering?
The first question, what is being gathered, will depend on your particular case, of course. The examples given here were written to gather lighting data: illuminance and color temperature. Your characteristics will vary.

#### When Is Each Set of Sensor Readings Taken?
It's useful to attach a timestamp to each set of sensor readings, and it's useful to use some time standards. The simplest solution is to let the server time stamp each reading, ad the servers in this collection do. However, there may be reasons to time stamp locally by the microcontroller. If that is the case, then you want to attach a real-time clock to the microcontroller, or use a controller with one built-in, like the Nano 33 IoT or the MKR boards. The [RTCZero]((https://www.arduino.cc/reference/en/libraries/rtczero/)) library lets you access that realtime clock, and the WiFi libraries let you set the clock by making a network time server request, using the command `WiFi.getTime()`. For more on time in connected systems, see [this page](timestamps).

#### Where Was the Data Gathered?

Unless you're using A GPS reader or some other machine-based way to determine your location, the simplest way is just to name the location in your data. Adding a parameter to your data with the location name is a good way to start.

#### Who (or What) Gathered the Data?

It's a good idea to always include a unique ID in your sensor readings so you can identify the device that took the reading, independent of the location or the time.  The servers described below filter requests by checking a unique ID (uid) in the request. If the ID that the client sends doesn't match one of the ones in a list called `knownClients`, the server responds with a 403 error and the data is not saved. 

You'll need to fill in an uid for your microcontroller for this to work. From any networked microcontroller, you can use the MAC address of the WiFi radio. You can see this in the Arduino example. 


