# DataLoggerHttpClientJSON Example

This Arduino sketch was written for the WiFi-enabled SAMD-based Arduino models, including the Nano 33 IoT, MKR1000 and MKR1010. It should run on other boards with some modification. 

# Program Flow

This sketch reads a sensor once a minute and sends the readings to a sensor once a minute. The details are as follows:

Data that the client sends:
* dateTime - an ISO8601-formatted string with the date and time in GMT, like so: `2021-05-19T17:16:00Z`
* uid - a unique 9-byte ID, sent as a hexadecimal ASCII string like so: `AA00BB11CC22DD33EE`
* lux - a reading of illuminance in lux, taken from a TCS34725 sensor
* ct - a reading of color temperature in degrees Kelvin, taken from the same sensor. 
* uptime - the time since the realtime clock was first set. This can be useful to check if the microcontroller has been reset. 

The data is stored in a JSON object called `body`, declared like so:

````arduino
JSONVar body;
````
It is created as a global variable using the [Arduino_JSON](https://github.com/arduino-libraries/Arduino_JSON) library. It is stringified before it's sent to the server in a POST request using the [ArduinoHttpClient](https://www.arduino.cc/reference/en/libraries/arduinohttpclient/) library. 

Any security-sensitive data, such as the WiFi configuration or server address, are stored in an extra file called `arduino_secrets.h` which you'll need to create. It looks like this:

````arduino
#define SECRET_SSID ""    //  your network SSID (name)
#define SECRET_PASS ""    // your network password 
#define SECRET_SERVER ""  // your server address
````

The server will either be the address of your node.js server, or `script.google.com` if you're using the Google script. Make sure not to include the `http://` or `https://` header.

## Global Variables

There are a few global declarations that you should consider, depending on your application:

````arduino
WiFiSSLClient netSocket;
````
This establishes an SSL socket to the server, for doing HTTPS requests. If you plan to use unencrypted HTTP requests instead, declare it as a `WiFiClient` instead. If you do, change the `port` variable from 443 to 80 or whatever your server is using instead (the node.js scripts in this repository use 8080, for example).

````arduino
String route = "/data";
````
The API route will change depending on your server. In the node.js scripts here, it's /data, but if you write your own server, it might change. If you use the Google script, it will be a variation on this: `https://script.google.com/macros/s/XXXXX/exec` where XXXX is a long unique string for your particular script. 

## Setup

The setup function initializes all the libraries and sets the first value in the `body` JSON using the unique ID of the board's crypto chip like so:

````arduino
ECCX08.begin();
uid = ECCX08.serialNumber();
body["uid"] = uid;
````

If you're using a board without the crypto chip, you'll need to declare your own unique ID. 

At the end of the setup, the sketch makes an initial attempt to connect to a WiFi network using the `connectToNetwork()` function. That function also calls `WiFi.getTime()` to set the real time clock. 

## Loop

The loop function starts by checking if the processor's still connected to WiFi. If not, it calls `connectToNetwork()`. Then it checks if there's a connection to the server. If not, it checks to see if a minute has passed since the last successful request to the server. If so, it reads the sensor using the `readSensor()` function, which puts the sensor readings in the `body` JSON. Then it makes an HTTPS POST request to the server. 

Next, the loop waits for any incoming response from the server, using `if (client.available())`. When there is a response, it reads the response code and body of the response. If the response is good (HTTP error code 200), it closes the request with `client.stop()` and updates the `lastRequestTime`  in order to track time until the next minute. 

## Secondary Functions

The other functions in this script are utilities: `readSensor()` reads the sensors and updates the `body` JSON. `getISOTimeString()` convers the realtime clock's current time to an ISO8601-compliant String. Finally, `connectToNetwork()` attempts to connect to the WiFi credentials found in `arduino_secrets.h` and when it connects, it makes an NTP time request to a time server, and updates the realtime clock with the result. 