// NOTE: this assumes valid inputs
// Inputs must be numbers 0-9 or UPPER CASE letter!
string ROT18(string message)
{
	string ROT18Msg = message;
	for (int i = 0; i < message.length(); i++){
		// Note we can assume upper case! message[i] = toupper(message[i]);
		char c = message[i];
		if ( c > 47 && c < 58){
			c += 25;
		}
		else {
			c += 18;
		}
		if (c > 90){
			c -= 43;
		}
		if (c > 57 && c < 65){
			c -= 7;
		}
		ROT18Msg[i] = c;
	}
	return ROT18Msg;
}
