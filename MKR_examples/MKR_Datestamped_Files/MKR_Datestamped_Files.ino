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
#include <RTCZero.h>
// The chip select pin.
// For MKRZero it's SDCARD_SS_PIN
// for mem shield, it's 4
const int  SD_CHIP_SELECT = SDCARD_SS_PIN;

// whether or not the SD card initialized:
bool SDAvailable = false;
// name of the file to write to:
String logFile;

// init realtime clock:
RTCZero rtc;

float maxBatteryVoltage = 4.3; // max battery voltage on the MKR series
long timestamp = 0;            // timestamp for SD logging, in ms
int interval = 1000;           // logging interval in ms

void setup() {
  Serial.begin(9600);
  // wait until serial monitor is open:
  while (!Serial);

  // initialize the realtime clock:
  rtc.begin();
  // set time and date from the compiler:
  setTimeFromCompile();
  setDateFromCompile();
  // get file name from date:
  logFile = fileNameFromDate();

  // initialize SD card:
  SDAvailable = SD.begin(SD_CHIP_SELECT);
  Serial.println("Card working: " + String(SDAvailable));
  // print a header to the SD card file:
  File dataFile = SD.open(logFile, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Battery voltage:,%:, elapsed seconds:");
    dataFile.close();
  }
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
      // update log file name if needed:
      logFile = fileNameFromDate();
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

String fileNameFromDate() {
  String result = "";
  if (rtc.getDay() < 10) result += "0";
  result += String(rtc.getDay());
  if (rtc.getMonth() < 10) result += "0";
  result += String(rtc.getMonth());
  if (rtc.getYear() < 10) result += "0";
  result += String(rtc.getYear());
  result += ".CSV";
  return result;
}


// set the rtc time from the compile time:
void setTimeFromCompile() {
  // get the compile time string:
  String compileTime = String(__TIME__);
  // break the compile time on the colons:
  int h = compileTime.substring(0, 2).toInt();
  int m = compileTime.substring(3, 5).toInt();
  int s = compileTime.substring(6, 8).toInt();

  // set the time from the derived numbers:
  rtc.setTime(h, m, s);
}

// set the rtc time from the compile date:
void setDateFromCompile() {
  String months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  // get the compile date:
  String compileDate = String(__DATE__);
  // get the date substring
  String monthStr = compileDate.substring(0, 3);

  int m = 0;    // variable for the date as an integer
  // see which month matches the month string:
  for (int i = 0; i < 12; i++) {
    if (monthStr == months[i]) {
      // the first month is 1, but its array position is 0, so:
      m = i + 1;
      // no need to continue the rest of the for loop:
      break;
    }
  }

  // get the day and year as substrings, and convert to numbers:
  int d = compileDate.substring(4, 6).toInt();
  int y = compileDate.substring(9, 11).toInt();
  // set the date from the derived numbers:
  rtc.setDate(d, m, y);
}
