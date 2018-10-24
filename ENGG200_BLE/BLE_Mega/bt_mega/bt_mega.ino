
#include <Arduino.h>
#include "interpretData.h"
#include "shared_types.h"


char r = '/';
char g = '/';
char b = '/';
//CanQuantities data = {r, g, b};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial2.begin(9600); need to uncomment whnechange back to Serial2
  //Serial2.println("conn

}

void loop() {
  // put your main code here, to run repeatedly:

  CanQuantities quantity = InterpretData.inputData();
    // if(Serial2.available()){
    // char c = Serial2.read();
    // Serial.println(c);
    // r = InterpretData.interpretRed(c,r);
    // g = InterpretData.interpretGreen(c,g);
    // b = InterpretData.interpretBlue(c,b);
    // CanQuantities quantity = {r, g, b};

    Serial.println(quantity.red);
    Serial.println(quantity.green);
    Serial.println(quantity.blue);
    Serial.println("");
    }

}
