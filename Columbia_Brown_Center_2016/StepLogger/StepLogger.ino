/*
  Step logger
  This sketch reads the battery level and an LIS3DH accelerometer
  on an Adalogger and saves their values to a comma-separated values (CSV) file.
  The step counter is crudely implemented using the accelerometer's tap
  detector.

  Written and tested on an Adalogger M0 board
  .
  created 22 Jan 2016
  updated 28 Feb 2016
  by Tom Igoe

*/

#include <SPI.h>
#include <SD.h>

#include <Wire.h>
#include <Adafruit_LIS3DH.h>

Adafruit_LIS3DH accelerometer = Adafruit_LIS3DH();
// Adjust this number for the sensitivity of the 'click' force
// this strongly depend on the range! for 16G, try 5-10
// for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80
#define CLICKTHRESHHOLD 40

long steps = 0;                   // step count. reset every card write
int minStepInterval = 50;         // min time between steps, in ms

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

  // 0x18 is the accelerometer's default I2C address:
  if (! accelerometer.begin(0x18)) {
    Serial.println("Couldn't start. Check wiring.");
    while (true);     // stop here and do nothing
  }

  // accelerometer range can be 2, 4, 8, or 16G:
  accelerometer.setRange(LIS3DH_RANGE_2_G);

  // 1 = single click only interrupt output
  accelerometer.setClick(1, CLICKTHRESHHOLD);
  delay(100);
}

void loop() {
  // read for clicks every 50 ms:
  byte click = accelerometer.getClick();
  if (click > 0) {
    steps++;
    Serial.println(steps);
  }
  delay(minStepInterval);

  // read sensors every 10 seconds
  if (millis()  - lastWriteTime >=  interval) {
    File logFile = SD.open(fileName, FILE_WRITE);   // open the log file
    if (logFile) {                                  // if you can write to the log file,
      digitalWrite(writeLed, HIGH);                 // turn on the write LED
      logFile.println(steps);             // print step count & newline to the log
      logFile.close();                    // close the file

      // for degugging only:
      Serial.println(steps);
      steps = 0;                          // reset step counter
      
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
