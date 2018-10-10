#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

String data;

void setup() {
	Serial.begin(9600); // data rate: 9600 baud
	pinMode(13, OUTPUT);
	lcd.begin(16, 2);
	lcd.blink();
	lcd.print("Hello");
}

void loop() {
	if (Serial.available() > 0) {
		delay(100);
		data = Serial.readString();
		Serial.print(data);
		Serial.print("\n");
		lcd.clear();
		lcd.print(data);
		if (data == '1') {
			digitalWrite(13, HIGH); // turn LED on
		}
		else if (data == '0') {
			digitalWrite(13, LOW); // turn LED off
		}
	}
}
