int PIN;

namespace bluetooth_uno {
	// create a connection between Uno and Mega
	bool initiateConnToMega();

	// encrypt data using ROT-18 (ext of ROT-13)
	int encryptData(int data);

	// add a checksum to data for transmission
	int addChecksum(int data);

	// calculate checksum
	int calcChecksum(int data);

	// transfer data to Mega
	int transferToMega(int data);
}
