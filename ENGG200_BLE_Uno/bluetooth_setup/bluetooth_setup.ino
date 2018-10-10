#include <SoftwareSerial.h>

int PIN = 9;
int butState = 0;
int state = 0;

void setup() {
  pinMode(PIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    state = Serial.read();
  }
  
  butState = digitalRead(PIN);
  // Serial.println(butState);
  
  if (butState == HIGH) {
    digitalWrite(1, butState);
  } else if (butState == LOW) {
    digitalWrite(1, butState);
  }
}
