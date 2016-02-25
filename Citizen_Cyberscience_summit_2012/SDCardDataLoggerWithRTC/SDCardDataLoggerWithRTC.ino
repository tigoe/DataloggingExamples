/*
  SD card Temp & Pressure datalogger
 
 This example shows how to log data from 
 a BMP085 temperature and pressure sensor 
 to an SD card using the SD library. It incorporates
 a DS1307/DS3231 Realtime clock
 
 It also uses the Adafruit DHT sensor library from 
 https://github.com/adafruit/DHT-sensor-library
 and the Adafruit Realtime Clock library:
 https://github.com/adafruit/RTClib
 And the BMP085 library:
 https://github.com/adafruit/BMP085-Library
 
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 * BMP085 sensor connected as follows:
 ** Vcc to 3.3V
 ** Ground to Ground
 ** SCL to A4
 ** SDA to A5
  * DS3231 Raltime Clock connected as follows:
 ** Vcc to 5V
 ** Ground to Ground
 ** SCL to A4
 ** SDA to A5
 
 created  20 Feb 2012
 by Tom Igoe
 
 This example code is in the public domain, subject to the licenses
 of the libraries used.
 
 */

#include <SD.h>
#include <BMP085.h>
#include <Wire.h>
#include "RTClib.h"

BMP085 bmp;              // library for the sensor
RTC_DS1307 RTC;          // library for the Realtime Clock

const int interval = 10; // the interval between reads, in seconds
int lastReadTime = 0;    // the last time you read the sensor, in seconds


// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 4; 
char fileName[] = "datalog.csv";    // filename to save to

void setup() {
  // start serial:
  Serial.begin(9600);
  // start the realtime clock:
  Wire.begin();
  RTC.begin();
  
  // if the clock isn't running, set the time:
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  // start the sensor and initialize the SD card:
  bmp.begin();
  startSDCard();
}

void loop()
{
  // Get the current time from the RTC:
  DateTime now = RTC.now();

// if ten seconds have passed:
  if (now.second() > lastReadTime + interval) {
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure();

    // open the file:
    File dataFile = SD.open(fileName, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
      // convert time to a string, d/m/y h:m:s:
      String timeStamp = readTimestamp(now);
      dataFile.print(timeStamp);
      dataFile.print("\t");
      dataFile.print(pressure);
      dataFile.print("\t");
      dataFile.println(temperature);
      // print to the serial port too:
      Serial.print(timeStamp);
      Serial.print("\t");
      Serial.print(pressure);
      Serial.print("\t");
      Serial.println(temperature);
      // update the last read time:
      lastReadTime = now.second();
      // close the datafile:
      dataFile.close();
    }  
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.csv");
    } 
  }
}


boolean startSDCard() {
  boolean result = false;
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    result = false;
  } 
  else {
    Serial.println("card initialized.");
    File dataFile = SD.open(fileName, FILE_WRITE);
    // open the file:
    if (dataFile) {
      dataFile.println("Date/Time\tPressure(Pa)\tTemperature");
      dataFile.close();
    }
  }  
  return result;
}


String readTimestamp(DateTime thisTime) {
  // concatenate a string with all the parts of the date and time:
  String result = String(thisTime.day());
  result +="/";
  result +=String(thisTime.month());
  result +="/";
  result +=String(thisTime.year());
  result +=" ";
  result +=String(thisTime.hour());
  result +=":";
  result +=String(thisTime.minute());
  result +=":";
  result +=String(thisTime.second());
  return result; 
}





