/*
  GA1AS202 Light sensor

   reads sensor on analog inout, outputs light value in lux

   Slight rearrangement of  Bill Earl's example for Adafruit, 
   with more notes.

   This version assumes a 3.3V supply for the board, not 5V.
   If you're using a 5V board, attach AREF to 3.3V and add this to setup:
   analogReference(EXTERNAL);

   modified 22 Jan 2016
   by Tom Igoe
 */
 
void setup() {
  Serial.begin(9600);   // initialize serial communication
}

void loop() {
  // read the sensor:
  int lightSensor = analogRead(A0);
   /*
     Top of sensor range is 100000 (10^5) lux.
     (the 5.0 below is the exponent, NOT 5V)
     Lux changes on a log10 scale, so you need to 
     use the power (pow()) function to do the conversion:
  */
  float lux = lightSensor * 5.0 / 1024;
  lux = pow(10, lux);
  Serial.println(lux);
  delay(100);
}
