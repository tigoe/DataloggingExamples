/*
  Datalogger client

  Connects to a server using HTTPS and uploads data.
  Also handles HTTP error 302 redirects from Google Sheets scripts. 
  For more on this, see https://developers.google.com/apps-script/guides/content#redirects

  This client works with the google sheets datalogger found in this repository

  Works with MKR1010, MKR1000, Nano 33 IoT
  Uses the following libraries:
   http://librarymanager/All#WiFi101  // use this for MKR1000
   http://librarymanager/All#WiFiNINA  // use this for MKR1010, Nano 33 IoT
   http://librarymanager/All#ArduinoHttpClient
   http://librarymanager/All#Arduino_JSON
   http://librarymanager/All#Adafruit_TCS34725 (for the sensor)

  In the arduino_secrets.h file:
  #define SECRET_SSID ""           //  your network SSID (name)
  #define SECRET_PASS ""           // your network password 
  #define SECRET_DEPLOYMENT_ID  "" // your Google scripts deployment ID

  created 23 May 2021
  updated 8 Apr 2023
  by Tom Igoe
*/
// include required libraries and config files
#include <SPI.h>
//#include <WiFi101.h>        // for MKR1000 modules
#include <WiFiNINA.h>  // for MKR1010 modules
#include <ArduinoHttpClient.h>
// for simplifying JSON formatting:
#include <Arduino_JSON.h>
// I2C and light sensor libraries:
#include <Wire.h>
#include <Adafruit_TCS34725.h>
// network names and passwords:
#include "arduino_secrets.h"

// network socket to server. For HTTP instead of HTTPS,
// use WiFiClient instead of WiFiSSLClient:
WiFiSSLClient netSocket;
// Server port. For HTTP instead of HTTPS, use 80 instead of 443:
const int port = 443;
// server name and API route:
const char server[] = "script.google.com";
String route = "/macros/s/DEPLOYMENT/exec";
// Make a HTTP client:
HttpClient client(netSocket, server, port);
// a JSON variable for the body of your requests:
JSONVar data;

// the content type to post data to server:
const char contentType[] = "application/json";
// set a location for the sensor:
String location = "north window";

// request timestamp in ms:
long lastRequestTime = 0;
// interval between requests, in ms:
int sendInterval = 120 * 1000L;
// initialize the light sensor:
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_300MS, TCS34725_GAIN_1X);

void setup() {
  Serial.begin(9600);  // initialize serial communication
  // if serial monitor is not open, wait 3 seconds:
  if (!Serial) delay(3000);
  // get the MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  String macAddress = "";
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      macAddress += "0";
    }
    macAddress += String(mac[i], HEX);
  }
  // add it to the data JSON for the requests to the server:
  data["uid"] = macAddress;
  // add a location for the sensor:
  data["location"] = location;
Serial.println(macAddress);
  // update the Google scripts deployment ID. Only needed for Google apps script:
  route.replace("DEPLOYMENT", SECRET_DEPLOYMENT_ID);
  // attempt to connect to network:
  connectToNetwork();
  // read the sensor:
  readSensor();
  // print latest reading, for reference:
  Serial.println(JSON.stringify(data));
  // make a post request:
  client.post(route, contentType, JSON.stringify(data));
}

void loop() {
  // if you disconnected from the network, reconnect:
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
  }


  // If the request interval has passed:
  if (millis() - lastRequestTime >= sendInterval) {

    // read the sensor
    readSensor();
    // print latest reading, for reference:
    Serial.println(JSON.stringify(data));
    // make a post request:
    client.post(route, contentType, JSON.stringify(data));
  }

  // If there is a response available, read it:
  if (client.available()) {
    // read the status code of the response
    int statusCode = client.responseStatusCode();

    // (google sheets returns a 302 redirect, for more on this
    // see https://developers.google.com/apps-script/guides/content#redirects
    bool requestComplete = false;
    // keep trying until you get a code 200:
    do {
      Serial.print("HTTP status code: ");
      Serial.println(statusCode);
      switch (statusCode) {
        case 200:
          lastRequestTime = millis();
          requestComplete = true;
          // close the request:
          client.stop();
          break;
        case 302:
          // make a redirect request:
          statusCode = redirectRequest();
          break;
        default:
          // unknown status code.
          // If you need to respond to other codes, put them here.
          // current code assumes anything other than a redirect completes the request:
          requestComplete = true;
          break;
      }
    } while (!requestComplete);
  }
}

/*
  readSensor. You could replace this with any sensor, as long as
  you put the results into the data JSON object
*/
void readSensor() {
  // get lux and color temperature from sensor:
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);
  // update elements of request data JSON object:
  data["lux"] = lux;
  data["ct"] = colorTemp;
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
        Serial.print("Redirecting to: ");
        Serial.print(tempServer);
        Serial.println(tempRoute);
      }
    }
  }
  // make a new client to make the request for the result:
  HttpClient redirectClient(netSocket, tempServer, port);
  int tempResponseCode = 0;

  // change from POST to GET, per Google's instructions:
  redirectClient.get(tempRoute);
  tempResponseCode = redirectClient.responseStatusCode();
  // print out the response:
  String response = redirectClient.responseBody();
  Serial.print("Redirect body: ");
  Serial.println(response);
  redirectClient.stop();
  return tempResponseCode;
}

void connectToNetwork() {
  // try to connect to the network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect to: " + String(SECRET_SSID));
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  Serial.println("connected to: " + String(SECRET_SSID));

  IPAddress ip = WiFi.localIP();
  Serial.print(ip);
  Serial.print("  Signal Strength: ");
  Serial.println(WiFi.RSSI());
}
