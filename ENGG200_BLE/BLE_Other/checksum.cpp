//function checksum
//check the message for even parity
boolean checkSumcheck(sting message){
	int Sum = 0;
	for (int i = 0; i <message.length(); i++){
		char c = message[i];
		Sum += c % 2;
	}
	if ( Sum % 2 == 0){
		return true;
	}
	return false;
}

//add a checksum for even parity
string AddCheckSum(string message){
	int Sum = 0;
	char SumChar = 1;
	for ( int i = 0; i < message.length(); i++){
		char c = message[i]
		Sum += c % 2;
	}
	if (Sum % 2 == 0){
		SumChar = 0;
	}
	message += SumChar;
	return message;
}
