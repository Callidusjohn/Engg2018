#include <SoftwareSerial.h>
#include "bluetooth_uno.h"

static BluetoothUno bt;

void setup() {
  	bt.initiateConnToMega();
}

void loop() {
	bt.getInfo();
}
