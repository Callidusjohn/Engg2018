
#include <Arduino.h>
#include "interpretData.h"
#include "shared_types.h"
#include "bluetooth_mega.h"

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	Serial2.begin(9600);
	BluetoothMega.initiateConnToUno();
}

void loop() {
  	// put your main code here, to run repeatedly:
	if(Serial2.available()){
	    // if(Serial2.available()){
	    // char c = Serial2.read();
	    // Serial.println(c);
	    // r = InterpretData.interpretRed(c,r);
	    // g = InterpretData.interpretGreen(c,g);
	    // b = InterpretData.interpretBlue(c,b);
	    // CanQuantities quantity = {r, g, b};
		//BluetoothMega.getInfo();
		String data = BluetoothMega.getData();
		//Serial.println(data);
		CanQuantities cans = BluetoothMega.inputData(data);
		Serial.println(cans.red);
		Serial.println(cans.green);
		Serial.println(cans.blue);
	}
}
