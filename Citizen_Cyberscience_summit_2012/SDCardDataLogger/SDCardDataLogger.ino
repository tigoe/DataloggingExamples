/*
  SD card Temp & Humidity datalogger

  This example shows how to log data from
  a DHT11 temperature and humidity sensor
  to an SD card using the SD library.

  It also uses the Adafruit DHT sensor library from
  https://github.com/adafruit/DHT-sensor-library

  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
   DHT11 sensor connected as follows:
 ** Ground connected to 5
 ** Voltage connected to 8
 ** data out connected to 7
 ** 10K resistor connected from 7 to +5V

  created  9 Feb 2012
  by Tom Igoe

  This example code is in the public domain, subject to the licenses
  of the libraries used.

*/

#include <SD.h>
#include "DHT.h"

#define DHTPIN 5        // what pin the sensor is connected to
#define DHTTYPE DHT11   // Which type of DHT sensor you're using: 

#define TEMPERATURE 1   //  for the DHT sensor
#define HUMIDITY 0      // for the DHT sensor

// initialize the sensor:
DHT dht(DHTPIN, DHTTYPE);
const int interval = 10 * 1000; // the interval between reads, in ms
long lastReadTime = 0;        // the last time you read the sensor, in ms


// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 4;

void setup() {
  Serial.begin(9600);
  if (startSDCard() == true) {
    startSensor();
  }
}

void loop()
{
  // Get the current time in ms:
  long currentTime = millis();

  if (currentTime > lastReadTime + interval) {
    float humidity = readSensor (HUMIDITY);
    float temperature = readSensor(TEMPERATURE);

    // open the file:
    File dataFile = SD.open("datalog.csv", FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
      dataFile.print(humidity);
      dataFile.print("\t");
      dataFile.println(temperature);
      dataFile.close();
    }
    // print to the serial port too:
    Serial.print(humidity);
    Serial.print("\t");
    Serial.println(temperature);
    lastReadTime = millis();
    // }
    // if the file isn't open, pop up an error:
//    else {
//      Serial.println("error opening datalog.csv");
//    }
  }
}


boolean startSDCard() {
  boolean result = false;
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    result = false;
  }
  else {
    Serial.println("card initialized.");
    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println();
      dataFile.println("rH (%) \t temp. (*C)");
      dataFile.close();
      result = true;
    }
  }
  return result;
}


void startSensor() {
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
