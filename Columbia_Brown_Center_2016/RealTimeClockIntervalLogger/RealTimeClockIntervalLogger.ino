/*
   This example shows how to set the realtime clock
   and then to do something on an interval.
   The interval is set in seconds, using the global variable <interval>

    created 28 Feb 2016
    by Tom Igoe
*/

#include <RTCZero.h>

// set your initial time and date values here:
int hours = 0;
int minutes = 0;
int seconds = 0;
int days = 0;
int months = 0;
int years = 0;

RTCZero rtc;                    // make an instance of the RTC library
boolean logged = false;
int interval = 10;              // logging interval, in seconds

void setup() {
  Serial.begin(9600);           // enable serial communication
  rtc.begin();                  // start the real-time clock

  rtc.setDate(days, months, years);      // set the date
  rtc.setTime(hours, minutes, seconds);  // set the time
  printTime();                           // print the time
}

void loop() {
  // if the second is evenly divisible by the interval:
  if (rtc.getSeconds() % interval == 0 ) {
    // if you haven't already logged data in this second:
    if (logged == false) {
      // log some data
      logData();
      // set the data logged flag to true
      logged = true;
    }
  } else {
    // set the data logged flag to false
    logged = false;
  }
}

void logData() {
  // read the sensors here
  // save to the SD card here
  printTime();            // print the time
}

/*
   Prints the time as DD/MM/YY,HH:M:SS
*/
void printTime() {
  print2Digits(rtc.getDay());
  Serial.print("/");
  print2Digits(rtc.getMonth());
  Serial.print("/");
  print2Digits(rtc.getYear());
  Serial.print(",");
  print2Digits(rtc.getHours());
  Serial.print(":");
  print2Digits(rtc.getMinutes());
  Serial.print(":");
  print2Digits(rtc.getSeconds());
  Serial.println();
}

// prints single-digit numbers as 2-digit
void print2Digits(byte number) {
  if (number <= 9) {
    Serial.print("0");
  }
  Serial.print(number);
}

