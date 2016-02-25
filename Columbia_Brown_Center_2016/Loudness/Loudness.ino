
int threshold = 100;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("started");
}

void loop() {
  int sensorReading = analogRead(A0);
  int loudness = abs(512 - sensorReading);

  if (loudness > threshold) {
    Serial.println(loudness);
  }
}
