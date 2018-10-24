
#include <Arduino.h>
#include "interpretData.h"
#include "shared_types.h"

char c;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
    if(Serial2.available()){
    c = Serial2.read();
    char r = interpretRed(c);
    char g = interpretGreen(c);
    char b = interpretBlue(c);
    CanQuantities quantity = {r, g, b};
    
    }
}
