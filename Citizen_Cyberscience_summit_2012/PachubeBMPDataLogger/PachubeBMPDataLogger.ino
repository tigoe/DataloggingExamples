/*
  Pachube 
 This example shows how to log data from 
 a BMP085 temperature and pressure sensor 
 to an pachube.com. 
 
 It also uses the Adafruit DHT sensor library from 
 https://github.com/adafruit/DHT-sensor-library
 And the BMP085 library:
 https://github.com/adafruit/BMP085-Library
 
 The circuit:
 * Ethernet Arduino
 
 * BMP085 sensor connected as follows:
 ** Vcc to 3.3V
 ** Ground to Ground
 ** SCL to A4
 ** SDA to A5
  * DS3231 Raltime Clock connected as follows:
 ** Vcc to 5V
 ** Ground to Ground
 ** SCL to A4
 ** SDA to A5
 
 created  20 Feb 2012
 by Tom Igoe
 
 This example code is in the public domain, subject to the licenses
 of the libraries used.
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <BMP085.h>

BMP085 bmp;                   // the sensor library
const int interval = 3000;    // the interval between sensor reads, in ms
long lastReadTime = 0;        // the last time you read the sensor, in ms

byte mac[] = {
  0xDE,0xAD,0xBE,0xEF,0xFE,0xED}; 
IPAddress ip(192,168,1,24);

// initialize the library instance:
EthernetClient client;

long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;        // state of the connection last time through the main loop
const int postingInterval = 10*1000;  //delay between updates to Pachube.com

char apiKey[] = "YOUR_API_KEY_HERE";
long feed = 0000;    // fill in your key here

void setup() {
  Serial.begin(9600); 
  delay(500);
  //start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Configure manually:
    Ethernet.begin(mac, ip);
  }
  // print the Arduino's IP address:
  Serial.println(Ethernet.localIP());

  // set up the pins for the sensor, and initialize:
  Wire.begin();
  bmp.begin(); 
}

void loop() {
  // get the current time in ms:
  long currentTime = millis();

  if (currentTime > lastReadTime + postingInterval) {
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure();

    // print results:
    Serial.print("Pa,");
    Serial.print(pressure);
    Serial.print("\ntemp,");
    Serial.println(temperature);

    // if you're not connected, and ten seconds have passed since
    // your last connection, then connect again and send data:
    if(!client.connected()) {
      sendData(pressure, temperature);
    }
    // update the time of the most current reading:
    lastReadTime = millis();
  }


  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}



// this method makes a HTTP connection to the server:
boolean sendData(float thisPressure, float thisTemperature) {
  if (client.connected()) client.stop();

  // if there's a successful connection:
  if ( client.connect("api.pachube.com", 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request. 
    // fill in your feed address here:
    client.print("PUT /v2/feeds/" + String(feed));
    client.println(".csv HTTP/1.1");
    client.println("Host: api.pachube.com");
    client.print("X-PachubeApiKey:");
    client.println(apiKey);
    client.print("Content-Length: ");

    /*
     work out the length of the string:
     "pressure," = 9 + 
     digits of humidity value + 
     "temp," = 5 +
     digits of temperature value + 
     "\r\n" = 2
     */
    int length = 9 + countDigits(thisPressure,2) + 2 + 5 + countDigits(thisTemperature,2) + 2;

    // calculate the length of the sensor reading in bytes:
    client.println(length);

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.print("pressure,");
    client.println(thisPressure);
    client.print("temp,");
    client.println(thisTemperature);
    client.println();
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
    Serial.println("data uploaded");
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

// Counts digits of a floating point number, to calculate content length
// for an HTTP call.
// Based on Arduino's internal printFloat() function.

int countDigits(double number, int digits)  { 
  int n = 0;

  // Handle negative numbers
  if (number < 0.0)
  {
    n++; // "-";
    number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i) {
    rounding /= 10.0;
  }
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;

  while (int_part > 0) {
    int_part /= 10;
    n++;
  }
  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    n++; //"."; 
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    n ++; // += String(toPrint);
    remainder -= toPrint; 
  } 
  return n;
}





