/*
  Datalogger client

  Connects to a server using HTTPS and uploads data.
  Uses realtime clock on the SAMD21 (MKR boards and Nano 33 IoT) to
  keep time.
  This version also handles HTTP error 302 redirects
  from Google Sheets scripts. For more on this,
  see https://developers.google.com/apps-script/guides/content#redirects

  This client works with the google sheets datalogger found in this repository

  This version uses no Strings, since it's not displaying anywhere

  Works with MKR1010, MKR1000, Nano 33 IoT
  Uses the following libraries:
   http://librarymanager/All#WiFi101  // use this for MKR1000
   http://librarymanager/All#WiFiNINA  // use this for MKR1010, Nano 33 IoT
   http://librarymanager/All#ArduinoHttpClient
   http://librarymanager/All#Arduino_JSON
   http://librarymanager/All#RTCZero
   http://librarymanager/All#Adafruit_TCS34725 (for the sensor)

  created 23 May 2021
  updated 25 May 2021
  by Tom Igoe
*/
// include required libraries and config files
#include <SPI.h>
//#include <WiFi101.h>        // for MKR1000 modules
#include <WiFiNINA.h>         // for MKR1010 modules
#include <ArduinoHttpClient.h>
// for simplifying JSON formatting:
#include <Arduino_JSON.h>
// realtime clock module on the SAMD21 processor:
#include <RTCZero.h>
// I2C and light sensor libraries:
#include <Wire.h>
#include <Adafruit_TCS34725.h>
// include crypto chip library:
#include <ECCX08.h>
// network names and passwords:
#include "arduino_secrets.h"

// network socket to server. For HTTP instead of HTTPS,
// use WiFiClient instead of WiFiSSLClient:
WiFiSSLClient netSocket;
// server name:
const char server[] = SECRET_SERVER;
// Server port. For HTTP instead of HTTPS, use 80 instead of 443:
const int port = 443;
// API route (fill in from Google Scripts):
const char route[] = "/macros/s/AKfycbysoDKuDvbf0lDRYh1EwEpyKEP0mckc45g2Is2_UDgy5IR-2S5n61Jm7GSgMIBnanZaAA/exec";
// set the content type:
const char contentType[] = "application/json";

// the HTTP client is global so you can use it in multiple functions below:
HttpClient client(netSocket, server, port);
// initialize RTC:
RTCZero rtc;
unsigned long startTime = 0;

// a JSON variable for the body of your requests:
JSONVar body;

// request timestamp in minutes:
long lastRequestTime = 0;
// last second:
int lastSecond = 0;
// interval between requests, in minutes:
int sendInterval = 1;
// initialize the light sensor:
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);


void setup() {
  Serial.begin(9600);              // initialize serial communication
  // if serial monitor is not open, wait 3 seconds:
  if (!Serial) delay(3000);
  // start the realtime clock:
  rtc.begin();
  // start the crypto chip and use its serial number
  // as a unique ID:
  ECCX08.begin();

  // a unique ID string for this client.
  // since you're using the ECCX08 chip, the ID will be
  // a 9-byte number in hexadecimal format:
  // add it to the body JSON for the requests to the server:
  body["uid"] = ECCX08.serialNumber();
  // attempt to connect to network:
  connectToNetwork();
}

void loop() {
  // if you disconnected from the network, reconnect:
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
  }

  // If the client is not connected:
  if (!client.connected()) {
    // and the request interval has passed:
    if (abs(rtc.getMinutes() - lastRequestTime) >= sendInterval) {
      // read the sensor
      readSensor();
      // print latest reading, for reference:
      if (Serial) Serial.println(JSON.stringify(body));
      // make a post request:
      client.post(route, contentType, JSON.stringify(body));
    }
  }

  // If there is a response available, read it:
  if (client.available()) {
    // read the status code of the response
    int statusCode = client.responseStatusCode();
    if (Serial) {
      Serial.print("Status code: ");
      Serial.println(statusCode);
    }
    // (google sheets returns a 302 redirect, for more on this
    // see https://developers.google.com/apps-script/guides/content#redirects
    if (statusCode == 302) {
      // make a redirect request:
      int responseCode = redirectRequest();
      // timestamp the request if you got a good response
      if (responseCode == 200) {
        lastRequestTime = rtc.getMinutes();
      }
    }
    // close the request:
    client.stop();
  }
}

/*
  readSensor. You could replace this with any sensor, as long as
  you put the results into the body JSON object
*/
void readSensor() {
  // get lux and color temperature from sensor:
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  // update elements of request body JSON object:
  body["dateTime"] = rtc.getEpoch();
  body["lux"] = lux;
  body["ct"] = colorTemp;
  body["uptime"] = rtc.getEpoch() - startTime;
}

/*
  This function creates a new HTTPClient to get the result
  from the google script redirect
*/
int redirectRequest() {
  String tempServer;
  String tempRoute;

  // read the reesponse headers:
  while (!client.endOfHeadersReached()) {
    // if there's a header ready to be read, read it:
    if (client.headerAvailable()) {
      // if the name is "Location", it'll have the URL
      // that you need to call to get the real response:
      if (client.readHeaderName() == "Location") {
        // get the value of the header:
        String redirect = client.readHeaderValue();
        // parse from the word "script" to "/macros"
        // this will give you the server name:
        int start = redirect.indexOf("script");
        int end = redirect.lastIndexOf("/macros");
        tempServer = redirect.substring(start, end);

        // now parse from the word "/macros" to ">"
        // this will give you the route:
        start = redirect.indexOf("/macros");
        end = redirect.lastIndexOf("\">");
        tempRoute = redirect.substring(start, end);

        // print out the response server and route:
        if (Serial) {
          Serial.println(tempServer);
          Serial.println(tempRoute);
        }
      }
    }
  }
  // make a new client to make the request for the result:
  HttpClient redirectClient(netSocket, tempServer, port);
  int tempResponseCode = 0;

  // change from POST to GET, per Google's instructions:
  redirectClient.get(tempRoute);
  tempResponseCode = redirectClient.responseStatusCode();
  // print out the response and body:
  if (Serial) {
    Serial.print("Status code: ");
    Serial.println(tempResponseCode);
    String response = redirectClient.responseBody();
    Serial.print("body: ");
    Serial.println(response);
  }
  redirectClient.stop();
  return tempResponseCode;
}


void connectToNetwork() {
  // try to connect to the network:
  while ( WiFi.status() != WL_CONNECTED) {
    if (Serial) {
      Serial.print("Attempting to connect to: ");
      Serial.println(SECRET_SSID);
    }
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  if (Serial) Serial.println("connected");

  // set the time from the network:
  unsigned long epoch;
  do {
    if (Serial) Serial.println("Attempting to get network time");
    epoch = WiFi.getTime();
    delay(2000);
  } while (epoch == 0);
  // if startTime's never been set, set it:
  if (startTime == 0) startTime = epoch;
  // set the RTC:
  rtc.setEpoch(epoch);
  if (Serial) {
    Serial.println(rtc.getEpoch());
    IPAddress ip = WiFi.localIP();
    Serial.print(ip);
    Serial.print("  Signal Strength: ");
    Serial.println(WiFi.RSSI());
  }
}
