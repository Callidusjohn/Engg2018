
#include <Arduino.h>
#include "shared_types.h"
#include "bluetooth_mega.h"
#include "interpretData.h"

char c;

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
    char r = interpretData.interpretRed(c);
    char g = interpretData.interpretGreen(c);
    char b = interpretData.interpretBlue(c);
    CanQuantities quantity = {r, g, b};
    Serial.println(quantity.red);
    Serial.println(quantity.green);
    Serial.println(quantity.blue);
    Serial.println("");
    }

}
