/*
  Network Client Logger

  Listens for clients on popular web ports and logs their IP
  addresses to an SD file, along with the time and date

  created 18 April 2019
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFi101.h>
#include <SD.h>
#include <RTCZero.h>
#include "arduino_secrets.h"

// The chip select pin.
// For MKRZero it's SDCARD_SS_PIN
// for mem shield, it's 4
const int  SD_CHIP_SELECT = SDCARD_SS_PIN;

// whether or not the SD card initialized:
bool SDAvailable = false;
// name of the file to write to:
String logFile = "DATALOG.CSV";

RTCZero rtc;
unsigned long startTime;

// number of times the device has reconnected
int reconnects = 0;

WiFiServer http(80);
WiFiServer https(443);
WiFiServer httpAlt(8080);
WiFiServer ssh(22);
WiFiServer smtp(25);
WiFiServer smtps(587);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize the realtime clock:
  rtc.begin();

  SDAvailable = SD.begin(SD_CHIP_SELECT);
  Serial.println("Card working: " + String(SDAvailable));
  // print a header to the SD card file:
  File dataFile = SD.open(logFile, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Header");
    dataFile.close();
  }

  connectToNetwork();
}

void loop() {
  getClient(http);
  getClient(https);
  getClient(httpAlt);
  getClient(ssh);
  getClient(smtp);
  getClient(smtps);
}

void getClient(WiFiServer server) {
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    if (SDAvailable) {
      File dataFile = SD.open(logFile, FILE_WRITE);
      if (dataFile) {
        dataFile.print(getDateStamp());
        dataFile.print(",");
        dataFile.print(getTimeStamp());
        dataFile.print(",");
        dataFile.print(client.readString());
        dataFile.print(",");
        dataFile.println(client.remoteIP());
        dataFile.close();
      }
    } else {
      Serial.print(getDateStamp());
      Serial.print(",");
      Serial.print(getTimeStamp());
      Serial.print(",");
      Serial.print(client.readString());
      Serial.print(",");
      Serial.println(client.remoteIP());
    }
    client.print("Hello, ");
    client.println(client.remoteIP());
    // close the connection:
    client.stop();
  }
}


void connectToNetwork() {
  // try to connect to the network:
  while ( WiFi.status() != WL_CONNECTED) {
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  // You're connected, turn on the LED:
  digitalWrite(LED_BUILTIN, HIGH);

  // set the time from the network:
  unsigned long epoch;
  do {
    epoch = WiFi.getTime();
    delay(1000);
  } while (epoch == 0);

  rtc.setEpoch(epoch);

  //set the startTime as the connect time of the server:
  startTime = rtc.getEpoch();
  // increment the reconnect count:
  reconnects++;

}

// format the time as hh:mm:ss
String getTimeStamp() {
  String timestamp = "";
  if (rtc.getHours() <= 9) timestamp += "0";
  timestamp += rtc.getHours();
  timestamp += ":";
  if (rtc.getMinutes() <= 9) timestamp += "0";
  timestamp += rtc.getMinutes();
  timestamp += ":";
  if (rtc.getSeconds() <= 9) timestamp += "0";
  timestamp += rtc.getSeconds();
  return timestamp;
}

// format the date as dd-mm-yyyy:
String getDateStamp() {
  String datestamp = "";
  if (rtc.getDay() <= 9) datestamp += "0";
  datestamp += rtc.getDay();
  datestamp += "-";
  if (rtc.getMonth() <= 9) datestamp += "0";
  datestamp += rtc.getMonth();
  // add century:
  datestamp += "-20";
  if (rtc.getYear() <= 9) datestamp += "0";
  datestamp += rtc.getYear();
  return datestamp;
}
