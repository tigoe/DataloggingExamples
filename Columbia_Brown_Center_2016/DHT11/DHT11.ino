/*
  This example shows how to read data from
  a DHT11 temperature and humidity sensor.

  The circuit:
   DHT11 sensor connected as follows:
 ** Ground connected to ground
 ** Voltage connected to Vcc
 ** data out connected to 5
 ** 10K resistor connected from 5 to +5V

  created  9 Feb 2012
  modified 25 Feb 2016
  by Tom Igoe
  
*/

#include <SD.h>
#include "DHT.h"

#define DHTPIN 5        // what pin the sensor is connected to
#define DHTTYPE DHT11   // Which type of DHT sensor you're using: 

// initialize the sensor:
DHT dht(DHTPIN, DHTTYPE);
const int interval = 10 * 1000; // the interval between reads, in ms
long lastReadTime = 0;          // the last time you read the sensor, in ms

void setup() {
  Serial.begin(9600);
    dht.begin();
}

void loop() {
  // Get the current time in ms:
  long currentTime = millis();
// if the read delay interval has passed, read the sensor:
  if (currentTime > lastReadTime + interval) {
    float humidity = dht.readTemperature();
    float temperature = dht.readHumidity();
    
     // print to the serial port:
    Serial.print(humidity);
    Serial.print(",");
    Serial.println(temperature);
    lastReadTime = millis();
  }
}
