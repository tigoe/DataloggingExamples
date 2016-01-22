/*
  Step logger
  This sketch reads the battery level and an LIS3DH accelerometer
  on an Adalogger and saves their values to a comma-separated values (CSV) file.
  The step counter is crudely implemented using the accelerometer's tap
  detector.

  Written and tested on an Adalogger 32U4 board
  .
  created 22 Jan 2016
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

const int batteryPin = A9;        // battery volotage is on pin A9
const int chipSelect = 4;         // SPI chip select for SD card
const int writeLed = 8;           // LED indicator for writing to card
const int errorLed = 13;          // LED indicator for error
long lastWriteTime = 0;           // timestamp for last write attempt
long interval = 10000;            // time between readings
char fileName[] = "datalog.csv";  // filename to save on SD card

void setup() {
  Serial.begin(9600);

  // initialize LED pins:
  pinMode(writeLed, OUTPUT);
  pinMode(errorLed, OUTPUT);

  // check to see that the SD card is responding:
  if (!SD.begin(chipSelect)) {      // if not,
    digitalWrite(errorLed, HIGH);   // turn on error LED
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (true);
  }
  // if the card initialized successfully
  Serial.println("card initialized.");

  if (! accelerometer.begin(0x18)) {
    Serial.println("Couldn't start. Check wiring.");
    while (true);     // stop here and do nothing
  }

  accelerometer.setRange(LIS3DH_RANGE_2_G);

  // 1 = single click only interrupt output
  accelerometer.setClick(1, CLICKTHRESHHOLD);
  delay(100);

  // add header columns to file:
  saveToFile("Steps");
}

void loop() {
  // read for clicks every 50 ms:
  byte click = accelerometer.getClick();
  if (click > 0) {
    steps++;
    Serial.println(steps);
  }

  delay(minStepInterval);

  // write to SD card every 10 seconds
  if (millis()  - lastWriteTime >=  interval) {
    // make a string to print to the data file and save it:
    String reading = String(steps);
    // if you successfully saved, reset the step counter:
    if (saveToFile(reading)) {
      steps = 0;
    }

    // update the last attempted save time:
    lastWriteTime = millis();
  }
}

boolean saveToFile(String dataString) {
  boolean result = false;
  // open the file:
  File dataFile = SD.open(fileName, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    digitalWrite(errorLed, LOW);    //all is good, make sure error LED is off
    digitalWrite(writeLed, HIGH);   // turn on write LED
    dataFile.println(dataString);   // write data to file
    dataFile.close();               // close file
    digitalWrite(writeLed, LOW);    // turn off write LED
    result = true;                  // you made a successful write
  } else {
    digitalWrite(errorLed, HIGH);   // couldn't write, turn on error LED
    result = false;                 // you failed to write
  }
  return result;
}

