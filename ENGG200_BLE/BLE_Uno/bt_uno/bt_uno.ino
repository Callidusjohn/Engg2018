#include <SoftwareSerial.h>
#include "bluetooth_uno.h"

void setup() {
  	BluetoothUno.initiateConnToMega();
}

void loop() {
	String message = "RGB123";
	if (message) {
		message = BluetoothUno.encryptData(message);
		BluetoothUno.transmitToMega(message);
	}
	BluetoothUno.getInfo();
}
