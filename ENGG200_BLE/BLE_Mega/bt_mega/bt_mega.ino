
#include <Arduino.h>
#include "interpretData.h"
#include "shared_types.h"

//CanQuantities data = {r, g, b};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);
  //Serial2.println("conn

}

void loop() {
  // put your main code here, to run repeatedly:
    if(Serial2.available()){
    char c = Serial2.read();
    Serial.println(c);
    char r = InterpretData.interpretRed(c);
    char g = InterpretData.interpretGreen(c);
    char b = InterpretData.interpretBlue(c);
    CanQuantities quantity = {r, g, b};
    Serial.println(quantity.red);
    Serial.println(quantity.green);
    Serial.println(quantity.blue);
    Serial.println("");
    }

}
