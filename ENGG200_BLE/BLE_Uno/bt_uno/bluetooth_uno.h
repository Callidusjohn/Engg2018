#include <Arduino.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
//#include <Serial.h>

static struct BluetoothUno {

	// create a connection between Uno and Mega
	void initiateConnToMega();

	// transfer data to Mega
 	void transmitToMega(String data);

	// check if any data is coming in from the Mega
	String checkForMegaData();

	// get data from the Mega
	String getData();

	// encrypt data using ROT-18 (ext of ROT-13)
	String encryptData(String data);

	// add a checksum to data for transmission
	int addChecksum(String message);

	// calculate checksum
	boolean calcChecksum(String message);

	// ##########################
	// ##### TEST FUNCTIONS #####
	// ##########################

    String prepareForMega(String data);

	void transmitToMega(int data);

	void lcd_errorChecking(int data);

	void getInfo();

	int PIN;
	AltSoftSerial BTSerial;
	bool NL = true;
	char c = ' ';
} BluetoothUno;
