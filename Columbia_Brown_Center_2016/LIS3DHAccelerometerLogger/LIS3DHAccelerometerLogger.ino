/*
    Adafruit LIS3DH accelerometer logger
    breakout board (https://www.adafruit.com/products/2809)

  This sketch reads the LIS3DH accelerometer
  on an Adalogger and saves their values to a comma-separated values (CSV) file.

  Written and tested on an Adalogger M0 board
  .
    Note: this code sets the accelerometer to 2G (aka 2x the acceleration
    due to gravity). The accelerometer has a 14-bit range, or -8193 to 8192.
    So at 1G, when the full force of gravity is on one of the axes,
    the reading should be around 4095 for that axis (or -4096, depending on
    the orientation).

    modified 26 Feb 2016
    by Tom Igoe
*/

#include <SD.h>
#include <Wire.h>
#include <Adafruit_LIS3DH.h>

Adafruit_LIS3DH accelerometer = Adafruit_LIS3DH();

const int chipSelect = 4;         // SPI chip select for SD card
const int cardDetect = 7;          // pin that detects whether the card is there
const int writeLed = 8;           // LED indicator for writing to card
const int errorLed = 13;          // LED indicator for error
long lastWriteTime = 0;           // timestamp for last write attempt
long interval = 10000;            // time between readings
char fileName[] = "datalog.csv";  // filename to save on SD card

void setup() {
  Serial.begin(9600);

  // initialize LED and cardDetect pins:
  pinMode(writeLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(cardDetect, INPUT_PULLUP);

  // Stay in this loop until the card is inserted:
  while (digitalRead(cardDetect) == LOW) {
    Serial.println("Waiting for card...");
    digitalWrite(errorLed, HIGH);
    delay(750);
  }

  // check if the card initialized successfully:
  if (startSDCard() == true) {
    Serial.println("card initialized.");
    delay(100);
  } else {
    Serial.println("Card failed");
  }
  // open the log file:
  File logFile = SD.open(fileName, FILE_WRITE);
  // write header columns to file:
  if (logFile) {
    logFile.println("Temperature,Humidity");
    logFile.close();
  }

  if (! accelerometer.begin(0x18)) {
    Serial.println("Couldn't start accelerometer. Check wiring.");
    while (true);     // stop here and do nothing
  }
  // set accelerometer range. Can be 2, 4, 8, or 16G:
  accelerometer.setRange(LIS3DH_RANGE_2_G);
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

      accelerometer.read();      // get X Y and Z data at once
      // print to the log file:
      // Convert the readings to g's (1g = 9.8 m/s^2):
      logFile.print(convertReading(accelerometer.x));
      logFile.print(",");
      logFile.print(convertReading(accelerometer.y));
      logFile.print(",");
      logFile.println(convertReading(accelerometer.z));

      // for debugging only:
      Serial.print(convertReading(accelerometer.x));
      Serial.print(",");
      Serial.print(convertReading(accelerometer.y));
      Serial.print(",");
      Serial.println(convertReading(accelerometer.z));

      // update the last attempted save time:
      lastWriteTime = millis();
    }
    digitalWrite(writeLed, LOW);      // turn off the write LED
  }
}

float convertReading(int reading) {
  float divisor = 2 << (13 - accelerometer.getRange());
  float acceleration = (float)reading / divisor;
  return acceleration;
}

boolean startSDCard() {
  // check to see that the SD card is responding:
  if (!SD.begin(chipSelect)) {      // if not,
    digitalWrite(errorLed, HIGH);   // turn on error LED
    return false;
  }
  return true;
}

