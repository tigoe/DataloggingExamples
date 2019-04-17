/*
  Accelerometer wake
  Sets the realtime clock on any of the MKR or M0 boards
  using the compile time and compile date.
  Logs the time once per second.

  Uses an LIS3DH accelerometer as an interrupt
  to wake the processor as a simple motion detector

  created 25 Nov. 2018
  modified 16 April 2019
  by Tom Igoe
*/

#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <ArduinoLowPower.h>
#include <RTCZero.h>
#include <SPI.h>
#include <SD.h>

// tap threshold for accelerometer sensitivity tap force.
// A rough formula is (40 to 80) / range.
// Higher numbers = less sensitivity:
const int  TAP_THRESHOLD = 10;
// The chip select pin.
// For MKRZero it's SDCARD_SS_PIN
// for mem shield, it's 4
const int  SD_CHIP_SELECT = SDCARD_SS_PIN;

// init accelerometer:
Adafruit_LIS3DH accelerometer = Adafruit_LIS3DH();
// init realtime clock:
RTCZero rtc;

String logFile = "DATALOG.CSV";  // data file name
volatile bool tap = false;       // tap occurred?
volatile bool alarm = false;     // alarm occurred?
bool accelAvailable = false;     // accelerometer working?
bool SDAvailable = false;        // SD card slot working?

void setup() {
  Serial.begin(9600);
  // delay 1 second so sleep doesn't occur right away:
  delay(1000);
  // initialize LED pin:
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize SD card:
  SDAvailable = SD.begin(SD_CHIP_SELECT);
  // start the accelerometer. Address 0x18 for Adafruit breakout,
  // 0x19 for Sparkfun breakout:
  accelAvailable = accelerometer.begin(0x19);
  Serial.println("Card working: " + String(SDAvailable));
  Serial.println("Accelerometer working: " + String(accelAvailable));
  // if either is not working, stop:
  while (!accelAvailable || !SDAvailable) {
    // stop and do nothing
  }

  // open the data file:
  File dataFile = SD.open(logFile, FILE_WRITE);
  // write a header to the file:
  if (dataFile) {
    dataFile.println("date:,time:,tap:");
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

  // set accelerometer range (2, 4, 8, or 16G):
  accelerometer.setRange(LIS3DH_RANGE_2_G);

  // set tap:
  // 0 = no tap interrupt
  // 1 = single tap interrupt
  // 2 = double tap interrupt output
  // also set threshold.
  accelerometer.setClick(1, TAP_THRESHOLD);
  // set interrupt om digital pin 1:
  LowPower.attachInterruptWakeup(digitalPinToInterrupt(1), getTap, CHANGE);
}

void loop() {
  // if tap or alarm occurs, wake up and log:
  if (tap || alarm) {
    digitalWrite(LED_BUILTIN, HIGH);
    // make a string for the log:
    String reading = getDateStamp();
    reading += "," + getTimeStamp();
    reading += "," + String(tap);
    cardWrite(reading);
    // reset tap and alarm:
    tap = false;
    alarm = false;
  }

  // sleep after three seconds (millis() resets on sleep):
  if (millis() / 1000 > 3) {
    // turn the LED off:
    digitalWrite(LED_BUILTIN, LOW);
    LowPower.sleep();
  }
}

// interrupt service routine for tap:
void getTap() {
  tap = true;
}

// interrupt service routine for alarm:
void alarmWake() {
  alarm = true;
}

// write a string to the SD card:
void cardWrite(String message) {
  // If the SD card initialized correctly,
  // write to it:
  if (SDAvailable) {
    File dataFile = SD.open(logFile, FILE_WRITE);
    if (dataFile) {
      dataFile.println(message);
      dataFile.close();
    }
  }
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
