/*
  Temperature and humidity logger
  This sketch reads the temperature and humidity from a DHT11 sensor
  on an Adalogger and saves their values to a comma-separated values (CSV) file.

  Written and tested on an Adalogger M0 board
  .
  created 28 Feb 2016
  by Tom Igoe

*/


#include <SD.h>
#include "DHT.h"

#define DHTPIN 5        // what pin the sensor is connected to
#define DHTTYPE DHT11   // Which type of DHT sensor you're using: 

// initialize the sensor:
DHT dht(DHTPIN, DHTTYPE);

const int chipSelect = 4;         // SPI chip select for SD card
const int cardDetect = 7;          // pin that detects whether the card is there
const int writeLed = 8;           // LED indicator for writing to card
const int errorLed = 13;          // LED indicator for error
long lastWriteTime = 0;           // timestamp for last write attempt
long interval = 10000;            // time between readings
char fileName[] = "datalog.csv";  // filename to save on SD card

void setup() {
 Serial.begin(9600);   // initialize serial communication

  // initialize LED and cardDetect pins:
  pinMode(writeLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(cardDetect, INPUT_PULLUP);

  // startSDCard() blocks everything until the card is present
  // and writable:
  if (startSDCard() == true) {
    Serial.println("card initialized.");
    delay(100);
    // open the log file:
    File logFile = SD.open(fileName, FILE_WRITE);
    // write header columns to file:
    if (logFile) {
      logFile.println("Battery Voltage,Sensor Reading");
      logFile.close();
    }
  }

  // initialize the sensor:
  dht.begin();
}

void loop() {
  // if the cart's not there, don't do anything more:
  if (digitalRead(cardDetect) == LOW) {
    digitalWrite(errorLed, HIGH);
    return;
  }
  // turn of the error LED:
  digitalWrite(errorLed, LOW);

  // read sensors every 10 seconds
  if (millis()  - lastWriteTime >=  interval) {
    File logFile = SD.open(fileName, FILE_WRITE);   // open the log file
    if (logFile) {                                  // if you can write to the log file,
      digitalWrite(writeLed, HIGH);                 // turn on the write LED
      // read sensor:
      float humidity = dht.readTemperature();
      float temperature = dht.readHumidity();

      // print to the log file:
      logFile.print(humidity);
      logFile.print(",");
      logFile.println(temperature);
      logFile.close();                    // close the file

      // for debugging only:
      Serial.print(humidity);
      Serial.print(",");
      Serial.println(temperature);

      // update the last attempted save time:
      lastWriteTime = millis();
    }
    digitalWrite(writeLed, LOW);      // turn off the write LED
  }
}

boolean startSDCard() {
  // Wait until the card is inserted:
  while (digitalRead(cardDetect) == LOW) {
    Serial.println("Waiting for card...");
    digitalWrite(errorLed, HIGH);
    delay(750);
  }

  // wait until the card initialized successfully:
  while (!SD.begin(chipSelect)) {
    digitalWrite(errorLed, HIGH);   // turn on error LED
    Serial.println("Card failed");
    delay(750);
  }
  return true;
}

