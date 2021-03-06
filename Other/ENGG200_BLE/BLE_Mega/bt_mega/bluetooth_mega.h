#include <Arduino.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include "shared_types.h"
//#include <Serial.h>

static struct BluetoothMega {

	// create a connection between Uno and Mega
	void initiateConnToUno();

	// get data sent and return string form
	String getData();

	// transfer data to Mega
 	void transmitToUno(String data);

	// encrypt data using ROT-18 (ext of ROT-13)
	String encryptData(String data);

	// add a checksum to data for transmission
	int addChecksum(String message);

	// calculate checksum
	boolean calcChecksum(String message);

	void transmitToMega(int data);

	void lcd_errorChecking(int data);

	void getInfo();

	CanQuantities quantity;

	CanQuantities inputData(String data);



	int PIN;
	AltSoftSerial BTSerial;
	bool NL = true;
	char c = ' ';
	struct cans {
		unsigned char red;
		unsigned char green;
		unsigned char blue;
	};
} BluetoothMega;
