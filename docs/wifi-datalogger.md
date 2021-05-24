# Datalogging to a Server via WiFi

The examples described here log data to a server via WiFi using the WiFiNINA or WiFi101 library. These should all work on the network-enabled MKR boards, along with the Nano 33 IoT and other SAMD-based boards which have network capability. They can likely work on Espressif-based boards (ESP32, ESP8266) with some modification.

[This collection](https://github.com/tigoe/DataloggingExamples/tree/master/WiFiDatalogger) includes [ArduinoHttpClient](https://www.arduino.cc/reference/en/libraries/arduinohttpclient/) examples along with a [node.js](https://nodejs.org/) server script. There are also instructions for how to log data to a Google Sheets spreadsheet using [Google Apps script](https://developers.google.com/apps-script/reference/spreadsheet/spreadsheet-app), thanks to Arnab Chakravarty. 

For other WiFi  and ArduinoHttpClient examples, see [this repository](https://tigoe.github.io/Wifi101_examples/). 

## Arduino HTTP Client for Logging

The central app in this collection is the [DataLoggerHttpClientJSON](https://github.com/tigoe/DataloggingExamples/tree/master/WiFiDatalogger/DataLoggerHttpClientJSON). It is an Arduino-based client, written for the SAMD boards (MKR boards, Nano 33 IoT). It reads a sensor, saves the readings in a JSON object using the [Arduino_JSON](https://github.com/arduino-libraries/Arduino_JSON) library, and  makes HTTPS requests to a server once a minute. 

This example uses a light and color sensor, the AMS [TCS34725](https://ams.com/tcs34725), using [Adafruit's library](https://github.com/adafruit/Adafruit_TCS34725) for it. It sends the illuminance levels in lux, and the color temperature levels in degrees Kelvin. You can replace it with any sensor you want.  

In addition to the sensor data, the client also sends a timestamp, generated from the Arduino's [realtime clock](https://www.arduino.cc/reference/en/libraries/rtczero/), and a unique ID, read from the board's [ECCx08 crypto chip](https://www.arduino.cc/reference/en/libraries/arduinoeccx08/). There are other alternatives for both of these if using a differnt board, see below.

## Datalogging Server Applications

This client was originally written to communicate with an HTTP server written in node.js. Since the communications between server and client are all HTTP, it can be adapted to communicate with other web-based apps with little change.  

### Node.js Datalogger

The [node-datalogging-server](https://github.com/tigoe/DataloggingExamples/tree/master/WiFiDatalogger/node-datalogging-server)has a RESTful API that accepts data formatted as a JSON string in the body of a POST request, and that's what the client sends. You can also see all the records of the server by making a GET request.

The JSON data in the POST request should look like this:
````js
{ "uid": client ID, a 9-byte ID in hexadecimal format,
  "date": date in ISO8601 format}
````

The [ISO8601 time format](https://en.wikipedia.org/wiki/ISO_8601) looks like this: `2021-05-24T10:31:14Z`

You can also include any sensor characteristics that you want to add. The Arduino examples in this collection send light and color temperature levels in lux (`lux`) and degrees Kelvin (`ct`), respectively. The server doesn't check the names of the characteristics in the JSON data, so you can add anything you want. 

This server can be run on any host that can run node.js. You can see it running on [Glitch.com](https://glitch.com/) at [this link](https://glitch.com/edit/#!/tigoe-datalogger). It also includes a web-based client, as a test example. 

The node.js client filters requests by checking a uid in the JSON body of the POST request. If the UID that the client sends doesn't match one of the ones in a list called `knownClients`, the server responds with a 403 error and the data is not saved. You'll need to fill in the uid of your microcontroller for this to work. You can get it using the [ECCx08 crypto chip](https://www.arduino.cc/reference/en/libraries/arduinoeccx08/) library like so:

````arduino
  // start the crypto chip and use its serial number
  // as a unique ID:
  ECCX08.begin();
  String uid = ECCX08.serialNumber();
  Serial.println(uid);
  ````

There are two versions of the server, one of which saves the incoming data in an array, and another which appends the data to a text file. Both of these are simpler substitutes for a database. The data from [server-filewriter](https://github.com/tigoe/DataloggingExamples/blob/master/WiFiDatalogger/node-datalogging-server/server-fileWriter.js) script is more persistent because it's saved to a separate file. The data from the basic [server](https://github.com/tigoe/DataloggingExamples/blob/master/WiFiDatalogger/node-datalogging-server/server.js) script will be lost when you stop the script running.  

### Google Sheets Datalogger

[Google Apps script](https://developers.google.com/apps-script/reference/spreadsheet/spreadsheet-app) allows you to save data to a Google Sheets spreadsheet via HTTP. Arnab Chakravarty has a [tutorial on how it works](https://github.com/AbolTaabol/Arduino-GoogleSheet_Logger). Briefly, the steps are as follows:

1. Set up a spreadsheet
2. Give the sheet some column headers
3. Publish it to the web as a web page using the File -> Publish for Web menu
4. Set up a script using the Tools -> Script Editor menu
5. Write the script
6. Deploy it
7. Note the relevant URLs and modify the Arduino client to contact them. 

The [google-sheets-datalogger](https://github.com/tigoe/DataloggingExamples/tree/master/WiFiDatalogger/google-sheets-datalogger) script in this collection will accept both GET and POST requests. For the POST request, the data format is the same as the node.js server above:

````js
{ "uid": client ID, a 9-byte ID in hexadecimal format,
  "date": date in ISO8601 format}
````
As with the node.js server, you can also include any sensor characteristics that you want to add. The Arduino examples in this collection send light and color temperature levels in lux (`lux`) and degrees Kelvin (`ct`), respectively, and the script looks for those characteristics as names of the column headings in the spreadsheet. 

This script works a bit differently from the node.js server above. The names of your client's sensor characteristics must match the column headings in the spreadsheet.  When a client sends a request, the script reads the names of the column headings in the spreadsheet and compares the characteristics' key names from the JSON object with the column headings. If the client's data has a characteristic matching one of the column heads, the script inserts the data into that column. 

## Realtime Clock and Crypto Chip Alternatives