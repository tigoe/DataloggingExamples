/*
  Datalogger MQTT client - JSON

  Connects to an MQTT broker and uploads data.
  Uses realtime clock on the SAMD21 (MKR boards and Nano 33 IoT) to
  keep time.

  This version has a UDP listener for checking status.
  You can send the following commands to port 43770:
  uptime, readingCount, lastReading, bssid, mac, or restart.

  Works with MKR1010, MKR1000, Nano 33 IoT
  Uses the following libraries:
   http://librarymanager/All#WiFi101  // use this for MKR1000
   http://librarymanager/All#WiFiNINA  // use this for MKR1010, Nano 33 IoT
   http://librarymanager/All#ArduinoMqttClient
   http://librarymanager/All#Arduino_JSON
   http://librarymanager/All#RTCZero
   http://librarymanager/All#Adafruit_TCS34725 (for the sensor)

  created 13 Jun 2021
  by Tom Igoe
*/
// include required libraries and config files
#include <SPI.h>
//#include <WiFi101.h>        // for MKR1000 modules
#include <WiFiNINA.h>         // for MKR1010 modules and Nano 33 IoT modules
// for simplifying JSON formatting:
#include <Arduino_JSON.h>
// realtime clock module on the SAMD21 processor:
#include <RTCZero.h>
#include <ArduinoMqttClient.h>
// I2C and light sensor libraries:
#include <Adafruit_TCS34725.h>
#include <WiFiUdp.h>
// network names and passwords:
#include "arduino_secrets.h"

// initialize WiFi connection using SSL
// (use WIFiClient and port number 1883 for unencrypted connections):
WiFiSSLClient wifi;
MqttClient mqttClient(wifi);

WiFiUDP Udp;                    // UDP instance
unsigned int udpPort = 43770;   // local port to listen on


// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 8883;
char topic[] = "light-readings";
const char willTopic[] = "light-readings/will";
String clientID = "light-client-";
const char location[] = "NW _corner";

// initialize RTC:
RTCZero rtc;
unsigned long startTime = 0;

// a JSON variable for the body of your requests:
JSONVar body;

// timestamp for the sensor reading and send:
long lastSendTime = 0;

// interval between requests, in minutes:
int sendInterval = 1;
// time before broker should release the will, in ms:
long int keepAliveInterval =  sendInterval * 5 * 60 * 1000;
// initialize the light sensor:
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
// number of successful readings that have been sent:
unsigned long readingCount = 0;

void setup() {
  Serial.begin(9600);              // initialize serial communication
  // if serial monitor is not open, wait 3 seconds:
  if (!Serial) delay(3000);
  // start the realtime clock:
  rtc.begin();
  // add the MAC address to the sensor as an ID:
  body["uid"] = getMacAddress(1);
  // add the location:
  body["location"] = location;

  // set the credentials for the MQTT client:
  // set a will message, used by the broker when the connection dies unexpectantly
  // you must know the size of the message before hand, and it must be set before connecting
  String willPayload = "sensor died";
  bool willRetain = true;
  int willQos = 1;
  // add location name to the client:
  clientID += location;
  mqttClient.setId(clientID);
  mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);
  mqttClient.setKeepAliveInterval(keepAliveInterval);
  mqttClient.beginWill(willTopic, willPayload.length(), willRetain, willQos);
  mqttClient.print(willPayload);
  mqttClient.endWill();
}

void loop() {
  //if you disconnected from the network, reconnect:
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
  }

  udpCheck();       // listen for incoming UDP

  // if not connected to the broker, try to connect:
  if (!mqttClient.connected()) {
    Serial.println("reconnecting to broker");
    connectToBroker();
  } else {

    // If the client is not connected:  if (!client.connected()) {
    // and the request interval has passed:
    if (abs(rtc.getMinutes() - lastSendTime) >= sendInterval) {
      // read the sensor
      readSensor();
      // print it:
      Serial.println(JSON.stringify(body));
      // send:
      mqttClient.beginMessage(topic);
      // add the value:
      mqttClient.print(JSON.stringify(body));
      // send the message:
      mqttClient.endMessage();
      // increment the reading count:
      readingCount++;
      // take note of the time you make your request:
      lastSendTime = rtc.getMinutes();
    }
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
  body["timeStamp"] = rtc.getEpoch();
  body["lux"] = lux;
  body["ct"] = colorTemp;
  body["uptime"] = rtc.getEpoch() - startTime;
  body["readingCount"] = readingCount;
}

void connectToNetwork() {
  // try to connect to the network:
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect to: " + String(SECRET_SSID));
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  Serial.println("connected.");

  // set the time from the network:
  unsigned long epoch;
  do {
    Serial.println("Attempting to get network time");
    epoch = WiFi.getTime();
    delay(2000);
  } while (epoch == 0);

  // set the RTC:
  rtc.setEpoch(epoch);
  if (startTime == 0) startTime = rtc.getEpoch();
  IPAddress ip = WiFi.localIP();
  Serial.print(ip);
  Serial.print("  Signal Strength: ");
  Serial.println(WiFi.RSSI());
  Udp.begin(udpPort);
}

boolean connectToBroker() {
  // if the MQTT client is not connected:
  if (!mqttClient.connect(broker, port)) {
    // print out the error message:
    Serial.print("MOTT connection failed. Error no: ");
    Serial.println(mqttClient.connectError());
    // return that you're not connected:
    return false;
  }
  // once you're connected, you can proceed:
  mqttClient.subscribe(topic);
  // return that you're connected:
  return true;
}

void udpCheck() {
  String message;
  bool restarting = false;
  // if there's data available, read a packet
  if (Udp.parsePacket() > 0) {        // parse incoming packet

    while (Udp.available() > 0) {     // parse the body of the message
      message = Udp.readString();
    }
    // remove whitespace:
    message.trim();
    // default:
    String response = message;

    if (message == "uptime" ) {
      response = getUptime();
    }
    if (message == "readingCount" ) {
      response = readingCount;
    }
    if (message == "lastReading" ) {

      response = JSON.stringify(body);
    }
    if (message == "bssid" ) {
      // get the base station MAC address:
      response = getMacAddress(0);
    }
    if (message == "mac" ) {
      // get the  MAC address:
      response = getMacAddress(1);
    }
    if (message == "restart" ) {
      response = "restarting...";
      restarting = true;
    }

    // start a response packet:
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.println(response);
    Udp.endPacket();                   // finish and send packet
  }
  // restart:
  if (restarting) NVIC_SystemReset();
}

// format uptime:
String getUptime() {
  unsigned long upNow = rtc.getEpoch() - startTime;
  int upSecs = upNow % 60;
  int upMins = (upNow % 3600L) / 60;
  int upHours = (upNow % 86400L) / 3600;
  int upDays = (upNow % 31556926L) / 86400L;
  String uptime = format2Digits(upDays);

  uptime += " days, ";
  uptime += format2Digits(upHours);
  uptime += ": ";
  uptime += format2Digits(upMins);
  uptime += ": ";
  uptime += format2Digits(upSecs);

  return uptime;
}

// format digit string:
String format2Digits(int number) {
  String result = "";
  if (number < 10) {
    result += "0";
  }
  result += String(number);
  return result;
}

// format MAC address:
String getMacAddress(int which) {
  String result = "";
  byte mac[6];
  if (which == 1 ) {       // get MAC address of this device
    WiFi.macAddress(mac);
  } else if (which == 0) {  // get MAC address of router
    WiFi.BSSID(mac);
  }
  for (int b = 5; b >= 0; b--) {
    if (mac[b] < 16) {
      result += "0";
    }
    result += String(mac[b], HEX);
  }
  return result;
}
