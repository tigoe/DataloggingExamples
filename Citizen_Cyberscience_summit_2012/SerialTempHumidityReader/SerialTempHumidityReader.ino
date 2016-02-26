/*
Temp & Humidity reader
 
 This example shows how to read data from 
 a DHT11 temperature and humidity sensor 
 and print to the serial port.
 
 It  uses the Adafruit DHT sensor library from 
 https://github.com/adafruit/DHT-sensor-library
 	
 The circuit:
 * DHT11 sensor connected as follows:
 ** Ground connected to 5
 ** Voltage connected to 8
 ** data out connected to 7
 ** 10K resistor connected from 7 to +5V
 
 created  9 Feb 2012
 by Tom Igoe
 
 This example code is in the public domain, subject to the licenses
 of the libraries used.
 
 */

#include "DHT.h"

#define DHTPIN 7              // what pin the sensor is connected to
#define DHTTYPE DHT11         // Which type of DHT sensor you're using: 

#define DHT_GND 5             // ground pin of the sensor
#define DHT_VCC 8             // voltage pin of the sensor

#define TEMPERATURE 1         //  for the DHT sensor
#define HUMIDITY 0            // for the DHT sensor

DHT dht(DHTPIN, DHTTYPE);     // initialize the sensor:  
const int interval = 10*1000; // the interval between sensor reads, in ms
long lastReadTime = 0;        // the last time you read the sensor, in ms


void setup() {
  Serial.begin(9600); 
  startSensor();
  Serial.println("rH (%) \t temp. (*C)");
}

void loop()
{
  // get the current time in ms:
  long currentTime = millis();

  if (currentTime > lastReadTime + interval) {
    float temperature =  readSensor(TEMPERATURE);
    float humidity = readSensor (HUMIDITY);
    // print results:
    Serial.print(humidity);
    Serial.print("\t");
    Serial.println(temperature);
    // update the time of the most current reading:
    lastReadTime = millis();
  }
}

void startSensor() {
  // set up pins to power and read sensor:
  pinMode(DHT_VCC, OUTPUT);
  pinMode(DHT_GND, OUTPUT);
  digitalWrite(DHT_VCC, HIGH);
  digitalWrite(DHT_GND, LOW);
  // start sensor:
  dht.begin(); 
}

// get the sensor readings and concatenate them in a String:

float readSensor( int thisValue) {
  float result;

  if (thisValue == TEMPERATURE) {
    result = dht.readTemperature();
  } 
  else if (thisValue == HUMIDITY) {
    // read sensor:
    result = dht.readHumidity();   
  }

  // make sure you have good readings. If the reading
  // is not a number (NaN) then return an error:
  if (isnan(result)) {
    // an impossible result for either reading 
    // so it'll work as an error:
    result = -273.0; 
  }
  return result;
}
