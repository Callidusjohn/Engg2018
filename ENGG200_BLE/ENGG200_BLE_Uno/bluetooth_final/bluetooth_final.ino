#include <SoftwareSerial.h>
#include "bluetooth_uno.cpp"

//AltSoftSerial BTSerial;
bluetooth_uno bt;

void setup() {
  	bt.initiateConnToMega();
}

void loop() {
	bt.getInfo();
}
