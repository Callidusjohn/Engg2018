#include <SoftwareSerial.h>
#include "bluetooth_uno.h"

void setup() {
  	BluetoothUno.initiateConnToMega();
}

void loop() {
	BluetoothUno.getInfo();
}
