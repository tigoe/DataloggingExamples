/*
  This example shows how to read data from
  a PIR motion sensor.

  The sensor is attached to digital pin 5.
  This sensor typically has a 2-second reset time.

  created 25 Feb 2016
  by Tom Igoe
*/

int lastReading = 0;                  // previous reading from the sensor

void setup() {
  Serial.begin(9600);                 // initialize serial communication
  pinMode(5, INPUT);                  // make pin 5 an input
}

void loop() {

  int motionSensor = digitalRead(5);    // read the sensor
  if (motionSensor != lastReading) {    // if it's changed, then
    Serial.println("sensor changed");   // print that out
  }
  lastReading = motionSensor;           // save the current reading for next time
  delay(10);                            // delay  10 ms
}
