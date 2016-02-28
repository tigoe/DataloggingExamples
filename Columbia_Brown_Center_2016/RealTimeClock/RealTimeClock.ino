/*
   This example shows how to set the realtime clock
   using a the compiler time, and how to set an alarm
   to call a function every minute.

    created 16 Feb 2016
    by Tom Igoe
 */
#include <RTCZero.h>

RTCZero rtc;
const byte days = 28;
const byte months = 02;
const byte years = 16;

void setup() {
  pinMode(8, OUTPUT);             // enable the green LED
  digitalWrite(8, HIGH);          // turn the green LED on
  Serial.begin(9600);             // enable serial communication
while(!Serial);

  rtc.begin();                    // start the real-time clock
  setCompileTime();               // set the clock from the time you compiled
  printTime();                    // print the time

  rtc.setAlarmTime(0, 0, 0);      // set an alarm
  rtc.enableAlarm(rtc.MATCH_SS);  // enable the alarm, match seconds only
  rtc.attachInterrupt(logData);   // when the alarm happens, run logData
}

void loop() {
  // the loop isn't doing anything in this sketch
}

// this is called by the RTC alarm once a minute
void logData() {
  printTime();
}

/*
  This function sets the realtime clock using the time
  that the sketch was compiled on your PC. It sets the date
  from the global constants at the top of the sketch
*/
void setCompileTime() {
  // get the time from the compiler:
  String timeString = __TIME__;
  byte timeArray[3];    //  array for the elements of the time
  int t = 0;            // array counter

  // iterate over the string, reading the first number
  // then removing it from the string, until
  // the string is gone:
  while (timeString.length() > 0) {
    // read beginning of string, convert to integer:
    byte nextNumber = timeString.toInt();
    // save number in the array:
    timeArray[t] = nextNumber;
    // remove the first three chars of the array:
    timeString.remove(0, 3);
    // increment the counter:
    t++;
  }
  // set the time using the array and
  // the date using the constants from above:
  rtc.setTime(timeArray[0], timeArray[1], timeArray[2]);
  rtc.setDate(days, months, years);
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

