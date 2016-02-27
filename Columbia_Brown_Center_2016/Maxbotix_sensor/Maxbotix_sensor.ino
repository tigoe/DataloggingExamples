/*
  This example shows how to read data from
  a Maxbotix Ultrasonic ranger

  The sensor is attached to analog pin 0.
  The sensor's typical response is Vcc/512 per inch

  created 25 Feb 2016
  by Tom Igoe
*/

float Vcc = 5.0;     // supply voltage. On battery, you should calculate this dynamically
void setup() {
  Serial.begin(9600);                 // initialize serial communication
}

void loop() {

  float sensorReading = analogRead(A0);    // read the sensor
  float voltage = sensorReading * (Vcc / 1023.0);
  float distance = Vcc / 512.0 * sensorReading;
  Serial.print(sensorReading);
  Serial.print(",");
  Serial.print(voltage);
  Serial.print(",");
  Serial.println(distance);
  delay(50);
}
