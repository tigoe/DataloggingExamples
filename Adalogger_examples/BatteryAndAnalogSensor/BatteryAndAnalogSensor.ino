/*
  Battery and analog input logger
  This sketch reads the battery level and analog input A0
  on an Adalogger and saves their values to a comma-separated values (CSV) file.

  created 22 Jan 2016
  by Tom Igoe

*/

#include <SPI.h>
#include <SD.h>

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
  
// add header columns to file:
  saveToFile("Battery Voltage,Sensor Reading");
}

void loop() {
// read sensors every 10 seconds
  if (millis()  - lastWriteTime >=  interval) {
    // read battery voltage:
    float batteryVoltage = analogRead(batteryPin);
    // voltage is halved by the on-board voltage divider, 
    // so real voltage is * 2:
    batteryVoltage =  batteryVoltage * 2;
    // multiply by reference voltage (3.3) and divide by analog resolution:
    batteryVoltage = (batteryVoltage * 3.3) / 1024;

    // sensor input pin is 0 - 3.3V. Read and convert:
    float sensorVoltage = analogRead(A0);
    sensorVoltage = (sensorVoltage * 3.3) / 1024;

    // make a string to print to the data file and save it:
    String reading = String(batteryVoltage); 
    reading += ",";          
    reading += sensorVoltage;
    saveToFile(reading);
 
    // update the last attempted save time:
    lastWriteTime = millis(); 
  }
}

void saveToFile(String dataString) {
  // open the file:
  File dataFile = SD.open(fileName, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    digitalWrite(errorLed, LOW);    //all is good, make sure error LED is off
    digitalWrite(writeLed, HIGH);   // turn on write LED
    dataFile.println(dataString);   // write data to file
    dataFile.close();               // close file
    digitalWrite(writeLed, LOW);    // turn off write LED
  } else {
    digitalWrite(errorLed, HIGH);   // couldn't write, turn on error LED
  }
}

