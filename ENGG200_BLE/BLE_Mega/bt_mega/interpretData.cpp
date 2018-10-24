#include <Arduino.h>
#include "shared_types.h"
#include "bluetooth_mega.h"
#include "interpretData.h"

char InterpretData::interpretRed(char c) {
  	//check for red value
	if(c >= 50 && c <= 58) {
		//convert to number
		c = c-2;
		return c;
	}
	return;
}

char InterpretData::interpretGreen(char c) {
  //check for green value

  if(c >= 59 && c <= 67) {
  //convert to number
  c = c - 11;
  return c;
  }
  return;
}

char InterpretData::interpretBlue(char c) {
  //check for blue value
  if(c >= 68 && c <= 76) {
  //convert to number
  c = c - 20;
  return c;
   }
  return;
}

void InterpretData::inputData () {
  if(Serial2.available()){
    char c = Serial2.read();
    char r = interpretRed(c);
    char g = interpretGreen(c);
    char b = interpretBlue(c);
    struct CanQuantities quantity = {r, g, b};
  }
  return;
}
