/*
   This example shows how to read an analog input and determine the
   frequency. Based on Arduino.cc FrequencyMeter example

    created 16 Feb 2016
    by Tom Igoe
 */

#include <AudioFrequencyMeter.h>

AudioFrequencyMeter meter;

void setup() {
  Serial.begin(9600);                 // initialize serial communication
  meter.setBandwidth(70.00, 5000);    // Ignore frequency out of this range
  meter.begin(A0, 45000);             // Intialize A0 at sample rate of 45kHz
}

void loop() {
  float frequency = meter.getFrequency();   // read frequency
  if (frequency > 70 && frequency < 5000) { // if it's within range
    Serial.print(frequency);                // print it      
    Serial.println(" Hz");
  }
}
