#include <SoftwareSerial.h>
<<<<<<< HEAD
#include "bluetooth_uno.cpp"

//AltSoftSerial BTSerial;
bluetooth_uno bt;
=======
#include "bluetooth_uno.h"
>>>>>>> 2cf69718a14ab75bae90f3faa00ea28eef419ec5

void setup() {
  	BluetoothUno.initiateConnToMega();
}

void loop() {
	BluetoothUno.getInfo();
}
