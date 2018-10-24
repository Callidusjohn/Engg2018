#include <Arduino.h>
#include "shared_types.h"
#include "interpretData.h"

char InterpretData::interpretRed(char c, char r) {
  //check for red value

if(c >= 50 && c <= 58) {
  c =
  //convert to number
  c = c-2;
  return c;
  }
  return r;
}

char InterpretData::interpretGreen(char c, char g) {
  //check for green value

  if(c >= 59 && c <= 67) {
  //convert to number
  c = c - 11;
  return c;
  }
  return g;
}

char InterpretData::interpretBlue(char c, char b) {
  //check for blue value
  if(c >= 68 && c <= 76) {
  //convert to number
  c = c - 20;
  return c;
   }
  return b;
}

CanQuantities InterpretData::inputData () {

    char c = Serial2.read();
    Serial.println(c);
    r = interpretRed(c,r);
    g = interpretGreen(c,g);
    b = interpretBlue(c,b);
    if(r >= 48 && r <= 56 && g >= 48 && g <= 56 && b >= 48 && b <= 56) {
      return CanQuantities {r, g, b};
    }
    Serial.println(quantity.red);
    Serial.println(quantity.green);
    Serial.println(quantity.blue);
    Serial.println("");

  return;
}

//CanQuantities InterpretData::inputData()
