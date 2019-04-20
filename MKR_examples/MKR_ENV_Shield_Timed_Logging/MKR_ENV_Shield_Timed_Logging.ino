/*
  This example shows how to read the MKR env shield once a minute.
  and log it to an SD card, then go into standby mode.

  created 20 April 2019
  by Tom Igoe
*/

#include <RTCZero.h>
#include <SPI.h>
#include <SD.h>
#include <Arduino_MKRENV.h>

// The chip select pin.
// For MKRZero it's SDCARD_SS_PIN
// for MKRENV shield, it's 4
const int  SD_CHIP_SELECT = 4;

// init realtime clock:
RTCZero rtc;

String logFile = "DATALOG.CSV";  // data file name
bool SDAvailable = false;        // SD card slot working?
bool shieldAvailable = false;    // ENV shield working?
volatile bool logWritten = false;// log entry written?

float maxBatteryVoltage = 4.3; // max battery voltage on the MKR series

void setup() {
  Serial.begin(9600);
  // delay 1 second so sleep doesn't occur right away:
  delay(1000);
  // initialize LED pin:
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize SD card:
  SDAvailable = SD.begin(SD_CHIP_SELECT);
  Serial.println("Card working: " + String(SDAvailable));
  shieldAvailable = ENV.begin();
  Serial.println("ENV Shield working: " + String(shieldAvailable));
  // if either is not working, stop:
  while (!shieldAvailable || !SDAvailable) {
    // stop and do nothing
  }
  
  // open the data file:
  File dataFile = SD.open(logFile, FILE_WRITE);
  // write a header to the file:
  if (dataFile) {
    dataFile.println("date:,time, \
    battery voltage, %, temp (°C),\
    humidity (%), pressure (kPa), \
    illuminance (Lux), UVA, UVB, UVIndex");
    dataFile.close();
  }
  
  // initialize the realtime clock:
  rtc.begin();
  // set time and date from the compiler:
  setTimeFromCompile();
  setDateFromCompile();

  // set wakeup alarm time, once a minute:
  rtc.setAlarmTime(00, 00, 00);
  rtc.enableAlarm(rtc.MATCH_SS);
  rtc.attachInterrupt(alarmWake);
}

void loop() {
  // turn the LED on when awake:
  digitalWrite(LED_BUILTIN, HIGH);

  // if you haven't written to the log since being awake,
  // take readings and write to log:
  if (!logWritten) {
    // read the battery:
    int adcReading = analogRead(ADC_BATTERY);
    // convert to a voltage:
    float batteryVoltage = adcReading * (maxBatteryVoltage / 1023.0);
    // and a percentage:
    float percentage = (batteryVoltage / maxBatteryVoltage) * 100;

    float temperature = ENV.readTemperature();
    float humidity = ENV.readHumidity();
    float pressure = ENV.readPressure();
    float illuminance = ENV.readLux();
    float uva = ENV.readUVA();
    float uvb = ENV.readUVB();
    float uvIndex = ENV.readUVIndex();

    // If the SD card initialized correctly,
    // write to it:
    if (SDAvailable) {
      File dataFile = SD.open(logFile, FILE_WRITE);
      // convert all the readings to strings
      // and add them to the log entry:
      String logEntry = getDateStamp() + ",";
      logEntry += getTimeStamp() + ",";
      logEntry += String(batteryVoltage) + ",";
      logEntry += String(percentage) + ",";
      logEntry += String(temperature) + ",";
      logEntry += String(humidity) + ",";
      logEntry += String(pressure) + ",";
      logEntry += String(illuminance) + ",";
      logEntry += String(uva) + ",";
      logEntry += String(uvb) + ",";
      logEntry += String(uvIndex);
      // write the log entry to the file:
      dataFile.println(logEntry);
      dataFile.close();
      logWritten = true;
    }
  }
  // turn the LED off:
  digitalWrite(LED_BUILTIN, LOW);
  rtc.standbyMode();
}

void alarmWake() {
  logWritten = false;
}

// format the time as hh:mm:ss
String getTimeStamp() {
  String timestamp = "";
  if (rtc.getHours() <= 9) timestamp += "0";
  timestamp += rtc.getHours();
  timestamp += ":";
  if (rtc.getMinutes() <= 9) timestamp += "0";
  timestamp += rtc.getMinutes();
  timestamp += ":";
  if (rtc.getSeconds() <= 9) timestamp += "0";
  timestamp += rtc.getSeconds();
  return timestamp;
}

// format the date as dd-mm-yyyy:
String getDateStamp() {
  String datestamp = "";
  if (rtc.getDay() <= 9) datestamp += "0";
  datestamp += rtc.getDay();
  datestamp += "-";
  if (rtc.getMonth() <= 9) datestamp += "0";
  datestamp += rtc.getMonth();
  // add century:
  datestamp += "-20";
  if (rtc.getYear() <= 9) datestamp += "0";
  datestamp += rtc.getYear();
  return datestamp;
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
