/*
   This example shows how to set the realtime clock
   using a serial input string, formatted like this:
   TDD:MM:YY:HH:MM:SS
   The separators can be any non-numeric character

    created 16 Feb 2016
    by Tom Igoe
 */

#include <RTCZero.h>

RTCZero rtc;    // mane an instance of the RTC library

void setup() {
  pinMode(8, OUTPUT);     // enable the green LED
  digitalWrite(8, HIGH);  // turn the green LED on
  Serial.begin(9600);     // enable serial communication

  rtc.begin();            // start the real-time clock
  printTime();            // print the time
}

void loop() {
  // if a T comes in the serial port, assume it's followed
  // by a date/time string
  if (Serial.available() > 0) {
    if (Serial.read() == 'T') {
      readSerialTime();
    }
  }
}

/*
   This function expects a string like this:
    DD:MM:YY:HH:MM:SS
    The separators can be any non-numeric character
*/
void readSerialTime() {
  // read serial input until you get a newline:
  String timeString = Serial.readStringUntil('\n');
  byte dateTimeArray[6];  // array for the elements of the string:
  int t = 0;              //  array index counter

  // iterate over the string, reading the first number
  // then removing it from the string, until
  // the string is gone:
  while (timeString.length() > 0) {
    // read beginning of string, convert to integer:
    byte nextNumber = timeString.toInt();
    // save number in the array:
    dateTimeArray[t] = nextNumber;
    // remove the first three chars of the array:
    timeString.remove(0, 3);
    // increment the counter:
    t++;
  }
  // set the date and the time using the array:
  rtc.setDate(dateTimeArray[0], dateTimeArray[1], dateTimeArray[2]);
  rtc.setTime(dateTimeArray[3], dateTimeArray[4], dateTimeArray[5]);
  // print the time:
  printTime();
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

