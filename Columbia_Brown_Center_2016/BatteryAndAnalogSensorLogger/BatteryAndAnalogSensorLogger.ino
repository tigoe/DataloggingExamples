/*
  Battery and analog input logger
  This sketch reads the battery level and analog input A0
  on an Adalogger and saves their values to a comma-separated values (CSV) file.

  Written and tested on an Adalogger M0 board
  .
  created 22 Jan 2016
  modified 28 Feb 2016
  by Tom Igoe

*/

#include <SPI.h>
#include <SD.h>

const int batteryPin = A7;        // battery voltage is on pin A7
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
      float batteryVoltage = readBattery();         // read the battery voltage

      // sensor input pin is 0 - 3.3V. Read and convert:
      float sensorVoltage = analogRead(A0);
      sensorVoltage = (sensorVoltage * 3.3) / 1024;

      logFile.print(batteryVoltage);      // print battery voltage to the log
      logFile.print(",");                 // print comma to the log
      logFile.println(sensorVoltage);     // print sensor voltage & newline to the log
      logFile.close();                    // close the file

      // for debugging only:
      Serial.print(batteryVoltage);
      Serial.print(",");
      Serial.println(sensorVoltage);

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

float readBattery() {
  // read battery voltage:
  float batteryV = analogRead(batteryPin);
  // voltage is halved by the on-board voltage divider,
  // so real voltage is * 2:
  batteryV =  batteryV
              * 2;
  // multiply by reference voltage (3.3) and divide by analog resolution:
  batteryV = (batteryV * 3.3) / 1024;
  return batteryV;
}
