/*
  MKR battery test with LowPower library and timed sleep
  This example shows how to put the MKR series boards to sleep
  for a timed interval using the LowPower library.
  When it wakes, it logs the battery level and elapsed time.
  Note that when you use LowPower.sleep() the millis() is reset on sleep,
  so you need to keep time in memory or use the realtime clock.
  The realtime clock is the better option, though this example
  is simpler, it just adds time to an elapsed seconds variable.

  created 16 April 2010
  by Tom Igoe
*/

#include <ArduinoLowPower.h>
#include <SPI.h>
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
float elapsedSeconds = 0.0;

void setup() {
  Serial.begin(9600);
  // delay 3 seconds so sleep doesnt occur right away:
  delay(3000);
  // initialize LED pin:
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize SD card:
  SDAvailable = SD.begin(SD_CHIP_SELECT);
  Serial.println("Card working: " + String(SDAvailable));
  // open the data file:
  File dataFile = SD.open(logFile, FILE_WRITE);
  // write a header to the file:
  if (dataFile) {
    dataFile.println("Battery voltage:,%:,elapsed seconds:");
    dataFile.close();
  }
}

void loop() {
  // turn the LED on when awake:
  digitalWrite(LED_BUILTIN, HIGH);
  // read the battery:
  int adcReading = analogRead(ADC_BATTERY);
  // convert to a voltage:
  float batteryVoltage = adcReading * (maxBatteryVoltage / 1023.0);
  // and a percentage:
  float percentage = (batteryVoltage / maxBatteryVoltage) * 100;

  // take a timestamp since wakeup before card write:
  timestamp = millis();
  // add the timestamp (in seconds) to the elapsed seconds
  elapsedSeconds += float(timestamp / 1000.0);
  // If the SD card initialized correctly,
  // write to it:
  if (SDAvailable) {
    File dataFile = SD.open(logFile, FILE_WRITE);
    dataFile.print(batteryVoltage);
    dataFile.print (",");
    dataFile.print(percentage);
    dataFile.print(",");
    dataFile.println(elapsedSeconds);
    dataFile.close();
  }
  // turn the LED off:
  digitalWrite(LED_BUILTIN, LOW);
  // sleep for a minute:
  LowPower.sleep(60000);
}
