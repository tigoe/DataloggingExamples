/*
  Battery check
  This sketch reads the battery level
  on an Adalogger M0

  Written and tested on an Adalogger M0 board.
  Based on Adafruit's battery read example.
  .
  created 22 Jan 2016
  modified 16 Feb 2016
  by Tom Igoe

*/

const int batteryPin = A7;        // battery voltage is on pin A7

void setup() {
  Serial.begin(9600);
}

void loop() {
  // read battery voltage:
  float batteryVoltage = analogRead(batteryPin);
  // voltage is halved by the on-board voltage divider,
  // so real voltage is * 2:
  batteryVoltage =  batteryVoltage * 2;
  // multiply by reference voltage (3.3) and divide by analog resolution:
  batteryVoltage = (batteryVoltage * 3.3) / 1024;
  Serial.println(batteryVoltage);
}
