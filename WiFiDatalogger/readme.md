# Datalogging to a Server via WiFi

The examples described here log data to a server via WiFi using a network-enabled microcontroller as a datalogging client. These examples should all work on the network-enabled MKR boards, along with the Nano 33 IoT and other SAMD-based boards which have network capability. They can likely work on Espressif-based boards (ESP32, ESP8266) with some modification.

[This collection](WiFiDatalogger/) includes [ArduinoHttpClient](https://www.arduino.cc/reference/en/libraries/arduinohttpclient/) examples along with a [node.js](https://nodejs.org/) server script. There are also instructions for how to log data to a Google Sheets spreadsheet using [Google Apps script](https://developers.google.com/apps-script/reference/spreadsheet/spreadsheet-app), thanks to Arnab Chakravarty. 

The scripts:
* [ArduinoHttpClient logger]()
* node.js server
* Google Apps script

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

## Arduino HTTP Client for Logging

The main client app in this collection is the [DataLoggerHttpClientJSON](https://github.com/tigoe/DataloggingExamples/tree/main/WiFiDatalogger/DataLoggerHttpClientJSON) client. It is an Arduino-based client, written for the SAMD boards (MKR boards, Nano 33 IoT, Uno WiFi). It reads a sensor, saves the readings in a JSON object using the [Arduino_JSON](https://github.com/arduino-libraries/Arduino_JSON) library, and makes HTTPS requests to a server once every two minutes. 

This example uses a light and color sensor, the AMS [TCS34725](https://ams.com/tcs34725), using [Adafruit's library](https://github.com/adafruit/Adafruit_TCS34725) for it. It sends the illuminance levels in lux, and the color temperature levels in degrees Kelvin. You can replace it with any sensor you want, however.  

In addition to the sensor data, the client also sends  the Arduino's MAC address as a client ID.  The id allows the server to filter out requests from clients that it doesn't already know. Any server on a public address always gets random requests, so it's a good idea to filter out the ones you don't want. The Arduino's WiFi MAC address does the job  for this. 

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

### Where Was the Data Gathered?

Unless you're using A GPS reader or some other machine-based way to determine your location, the simplest way is just to name the location in your data. Adding a parameter to your data with the location name is a good way to start.

#### Who (or What) Gathered the Data?

It's a good idea to always include a unique ID in your sensor readings so you can identify the device that took the reading, independent of the location or the time.  The servers described below filter requests by checking a unique ID (uid) in the request. If the ID that the client sends doesn't match one of the ones in a list called `knownClients`, the server responds with a 403 error and the data is not saved. 

You'll need to fill in an uid for your microcontroller for this to work. From any networked microcontroller, you can use the MAC address of the WiFi radio. You can see this in the Arduino example. 

## Datalogging Server Applications

The client above was originally to communicate with an HTTP/s server. Since the communications between server and client are all HTTP/s, the client can be adapted to communicate with other web-based apps with little change.  Linked here are two examples, [one written in node.js](/WiFiDatalogger/node-datalogging-server/readme.md), and the other written using Google Scripts and using a Google Sheets spreadsheet.


### Google Sheets Datalogger

![system diagram of a datalogger connected to a Google spreadsheet, as described below.](images/wifi-datalogger-google-sheets.png)

_Figure 2. System diagram of the Google apps script datalogger_

The [Google Apps script](https://developers.google.com/apps-script/reference/spreadsheet/spreadsheet-app) in this repository allows you to save data to a Google Sheets spreadsheet via HTTP. Arnab Chakravarty has a [tutorial on how it works](https://github.com/AbolTaabol/Arduino-GoogleSheet_Logger). For more background on Google Apps scripts, see the link above, or [this link](https://developers.google.com/apps-script/guides/web) which explains the web functions `doGet()` and `doPost()`, which are the main functions of this example. 

The structure of the system is similar to the node server, and is diagrammed in Figure 2. The microcontroller only has to change the URL and API route that it's sending data to, in order to send data to the Google apps script. The script takes the place of the node.js server, and writes to a Google Sheets spreadsheet instead of a text file.

What's great about the Google Apps scripting API is that it's just JavaScript, it lets you give your spreadsheet into a web service quickly,   and it gives you everything you might need to know about a given HTTP request in JSON, so it's easy to parse it out and put it into the cells of a spreadsheet. This lets you take advantage of all the things you can do with the data in a spreadsheet: sorting, graphing, filtering, and so forth.

Briefly, the steps to get started are as follows:

1. Set up a spreadsheet
2. Give the sheet some column headers
3. Publish it to the web as a web page using the File -> Publish for Web menu
4. Set up a script using the Tools -> Script Editor menu
5. Write the script
6. Deploy it
7. Note the relevant URLs and modify the Arduino client to contact them. 

The [google-sheets-datalogger](https://github.com/tigoe/DataloggingExamples/tree/main/WiFiDatalogger/google-sheets-datalogger) script in this collection will accept both GET and POST requests. For the POST request, the data format is the same as the node.js server above:

````js
{ "uid": client ID, a 9-byte ID in hexadecimal format,
  "date": date in ISO8601 format}
````
As with the node.js server, you can also include any sensor characteristics that you want to add. The Arduino examples in this collection send light and color temperature levels in lux (`lux`) and degrees Kelvin (`ct`), respectively, and the script looks for those characteristics as names of the column headings in the spreadsheet. 

This script works a bit differently from the node.js server above. The names of your client's sensor characteristics must match the column headings in the spreadsheet.  When a client sends a request, the script reads the names of the column headings in the spreadsheet and compares the characteristics' key names from the JSON object with the column headings. If the client's data has a characteristic matching one of the column heads, the script inserts the data into that column. For example, the Arduino client in this repository sends the following: `uid`,`dateTime`,`lux`,`ct`,`uptime`. So the Google sheet needs headings with the same names in order to capture it all. 


#### Google Script URLs

To connect to the Google script from the Arduino clients, you need to change the server address and the API route. The server address is always `script.google.com`. This is not the address of your actual spreadsheet, it's the address of the spreadsheet's script. 

The API route will depend on your script's deployment. The URL will change every time you deploy the script. To get it, click the Deploy button in the script editor, choose "Manage Deployments, and copy the Web URL of the current deployment. It will look something like this:

````
https://script.google.com/macros/s/XXXXXX/exec
````
You can see the server address mentioned above in this URL. The API route is the part after the server; in this case, it's `macros/s/XXXXX/exec`. It will be different for your script. Copy it, and paste it into the `route` global variable in the [DataLoggerHttpClientJSON](https://github.com/tigoe/DataloggingExamples/tree/main/WiFiDatalogger/DataLoggerHttpClientJSON) or [DataLoggerHttpClientJSON_redirect](https://github.com/tigoe/DataloggingExamples/tree/main/WiFiDatalogger/DataLoggerHttpClientJSON_redirect) sketch. Now your Arduino should be able to connect to the script.

#### Google Script Redirects

For security, Google scripts redirect to a one-time URL, generated with each new request, to respond to requests. Your HTTPS request will get a 302 response like this:

````sh
HTTP/2 302 
content-type: text/html; charset=UTF-8
access-control-allow-origin: *
cache-control: no-cache, no-store, max-age=0, must-revalidate
pragma: no-cache
expires: Mon, 01 Jan 1990 00:00:00 GMT
date: Mon, 24 May 2021 15:39:34 GMT
location: https://script.googleusercontent.com/macros/echo?user_content_key=XXXXXXXXXXXXXXXXXXXXX
````
You can see that one of the headers of the response is the location of this URL. When you get that, you need to make your request again, using the server and route in the redirect location. 

The script will still add your data to the sheet if it's valid, even when it gives you a 302 response. So you can use the [DataLoggerHttpClientJSON](https://github.com/tigoe/DataloggingExamples/tree/main/WiFiDatalogger/DataLoggerHttpClientJSON) and accept the 302 response if you want. The sketch will still work with the script, even if it's not reading the whole response. 

If you want the full response from the server, however, try the [DataLoggerHttpClientJSON_redirect](https://github.com/tigoe/DataloggingExamples/tree/main/WiFiDatalogger/DataLoggerHttpClientJSON_redirect) sketch. When this sketch gets the 302 response, it reads the headers, parses the new server and API route from the `location` field, makes the second request to that address, and gets the response back. 

For more on this, see the [Google script guide on redirects](https://developers.google.com/apps-script/guides/content#redirects). 




