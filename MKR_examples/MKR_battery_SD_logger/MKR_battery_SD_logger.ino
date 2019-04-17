/*
   Battery voltage logger for MKR Zero/1010 etc
 
  This sketch can be used for general datalogging as well, by replacing the
  battery read with your own sensor reading. 
  Also prints the SD card write time, for diagnostic reference
  
  For SD card access:
  - for MKR Zero, use chip select pin = SDCARD_SS_PIN
  - for MKR mem shield, chip select pin = 4
  - for many third party SD card readers, chip select pin 4 works too
  
   created 10 April 2019
   updated 16 April 2019
   by Tom Igoe
*/
#include <SD.h>
// The chip select pin.
// For MKRZero it's SDCARD_SS_PIN
// for mem shield, it's 4
const int  SD_CHIP_SELECT = SDCARD_SS_PIN;

// whether or not the SD card initialized:
bool SDAvailable = false;
// name of the file to write to:
String logFile = "DATALOG.CSV";

float maxBatteryVoltage = 4.3; // max battery voltage on the MKR series
long timestamp = 0;            // timestamp for SD logging, in ms
int interval = 1000;           // logging interval in ms

void setup() {
  Serial.begin(9600);
  // wait until serial monitor is open:
  while (!Serial);
  // initialize SD card:
  SDAvailable = SD.begin(SD_CHIP_SELECT);
  Serial.println("Card working: " + String(SDAvailable));
  // print a header to the SD card file:
  File dataFile = SD.open(logFile, FILE_WRITE);
  dataFile.println("Battery voltage:,%:, elapsed seconds:");
  dataFile.close();
}

void loop() {
  if (millis() - timestamp > interval) {
    // read the battery:
    int adcReading = analogRead(ADC_BATTERY);
    // convert to a voltage:
    float batteryVoltage = adcReading * (maxBatteryVoltage / 1023.0);
    // and a percentage:
    float percentage = (batteryVoltage / maxBatteryVoltage) * 100;
    // take a timestamp before card write:
    timestamp = millis();

    // If the SD card initialized correctly,
    // write to it:
    if (SDAvailable) {
      File dataFile = SD.open(logFile, FILE_WRITE);
      dataFile.print(batteryVoltage);
      dataFile.print (",");
      dataFile.print(percentage);
      dataFile.print(",");
      dataFile.println(float(timestamp / 1000.0));
      dataFile.close();
      // note the card write time:
      Serial.println("card write time: " + String(millis() - timestamp) + " ms");
    }
  }
}
