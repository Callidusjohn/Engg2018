#include "motor_drive.h"
#include "../CanIntake/can_intake.h"
//Metro MotorDrive::serialMetro = Metro(250);  // Instantiate an instance
//Metro MotorDrive::sensorMetro = Metro(100);

Chrono MotorDrive::chrono = Chrono(Chrono::MILLIS);

double MotorDrive::countLeft = 0;
double MotorDrive::countRight = 0;

int MotorDrive::dir = 1;

int MotorDrive::targetPos = 1000;

double MotorDrive::irRightAddition, MotorDrive::irLeftAddition, MotorDrive::driveLeftPidOutput, MotorDrive::driveRightPidOutput;
double MotorDrive::inputIR, MotorDrive::setPointIR, MotorDrive::outputFromIR;

//DRIVE PIDS
double MotorDrive::driveSetPoint;
double MotorDrive::outputRDrive;
double MotorDrive::outputLDrive;


//rgb stuff
int MotorDrive::frequency, MotorDrive::redFreq, MotorDrive::greenFreq, MotorDrive::blueFreq;
bool MotorDrive::sensorRed, MotorDrive::sensorGreen, MotorDrive::sensorBlue;

// servos
Servo MotorDrive::servoLeft;
Servo MotorDrive::servoRight;

int MotorDrive::throttleLeft, MotorDrive::throttleRight;

AutoPID MotorDrive::sensorPID = AutoPID(&inputIR, &setPointIR, &outputFromIR, OUTPUT_MIN_IR, OUTPUT_MAX_IR, KP_IR, KI_IR, KD_IR);
AutoPID MotorDrive::leftPID = AutoPID(&countLeft, &driveSetPoint, &outputLDrive, OUTPUT_MIN_IR, OUTPUT_MAX_IR, LEFT_KP, LEFT_KI, LEFT_KD);
AutoPID MotorDrive::rightPID = AutoPID(&countRight, &driveSetPoint, &outputRDrive, OUTPUT_MIN_IR, OUTPUT_MAX_IR, RIGHT_KP, RIGHT_KI, RIGHT_KD);

bool MotorDrive::has_read_a_color;
bool MotorDrive::color_reading_in_progress;
Chrono::chrono_t MotorDrive::disable_color_sensor_until;

MotorDrive::MotorDrive() 
{
	chrono.start();
	driveSetPoint = targetPos;
	pinMode(OUTPUT_PIN, OUTPUT);
	attachInterrupt(digitalPinToInterrupt(18), ISRleft, RISING);
	attachInterrupt(digitalPinToInterrupt(19), ISRright, RISING);
	pinMode(22, INPUT); // 18 and 22 come from left
	pinMode(23, INPUT); // 19 and 23 come from right
	pinMode(6, INPUT); //enable pin
	servoLeft.attach(4);  // attaches the servo on pin 9 to the servo object
	servoRight.attach(5);  // attaches the servo on pin 9 to the servo object
}

void MotorDrive::ISRleft() {
	if (digitalRead(22) == LOW) {
		countLeft = countLeft + 1;
	}
	else { countLeft = countLeft - 1; }
}
void MotorDrive::ISRright() {
	if (digitalRead(23) == HIGH) {
		countRight = countRight + 1;
	}
	else { countRight = countRight - 1; }
}

bool MotorDrive::inRange(int val, int minimum, int maximum)
{
	return ((minimum <= val) && (val <= maximum));
}

void MotorDrive::driveSomewhere() {
	if (CanIntake::needsMoreCans()) {
		dir = 1;
		if (CanIntake::needsMoreCans(CanType::red)) {
			requiredColor = CanType::red;
		}
		else if (CanIntake::needsMoreCans(CanType::green)) {
			requiredColor = CanType::green;
		}
		else if (CanIntake::needsMoreCans(CanType::blue)) {
			requiredColor = CanType::blue;
		}
		AsyncHandler.addCallback(&checkColorSensor);
	}
	else {
		dir = -1;
	}
	AsyncHandler.addCallback(&updatePIDValues, 100);
}

void MotorDrive::checkColorSensor() {
	if (color_reading_in_progress) return;
	if (chrono.hasPassed(disable_color_sensor_until)) {
		color_reading_in_progress = true;
		checkColorSensorPhase1();
	}
	else {
		AsyncHandler.addCallback(&checkColorSensor, disable_color_sensor_until - chrono.elapsed());
	}

}

void MotorDrive::checkColorSensorPhase1() {
	digitalWrite(S2, LOW);
	digitalWrite(S3, LOW);
	redFreq = pulseIn(sensorOut, LOW);
	redFreq = map(redFreq, 130, 555, 0, 255);
	if (redFreq < 50 && (redFreq < redFreq && redFreq < blueFreq)) {
		sensorRed = true;
		sensorBlue = false;
		sensorGreen = false;
		detectedColor = CanType::red;
	}
	AsyncHandler.addCallback(&checkColorSensorPhase2, 100);
}

void MotorDrive::checkColorSensorPhase2() {
	digitalWrite(S2, HIGH);
	digitalWrite(S3, HIGH);
	greenFreq = pulseIn(sensorOut, LOW);
	greenFreq = map(greenFreq, 190, 1300, 0, 255);
	if (greenFreq < 50 && (greenFreq < redFreq && greenFreq < blueFreq)) {
		sensorGreen = true;
		sensorBlue = false;
		sensorRed = false;
		detectedColor = CanType::green;
	}
	AsyncHandler.addCallback(&checkColorSensorPhase3, 100);
}

void MotorDrive::checkColorSensorPhase3() {
	digitalWrite(S2, LOW);
	digitalWrite(S3, HIGH);
	blueFreq = pulseIn(sensorOut, LOW);
	blueFreq = map(blueFreq, 55, 370, 0, 255);
	if (blueFreq < 50 && (blueFreq < greenFreq && blueFreq < redFreq)) {
		sensorBlue = true;
		sensorGreen = false;
		sensorRed = false;
		detectedColor = CanType::blue;
	}
	has_read_a_color = true;
	color_reading_in_progress = false;
	disable_color_sensor_until = chrono.elapsed() + 100;
	AsyncHandler.addCallback(&checkSensedColor);
}

void MotorDrive::checkSensedColor() {
	if (requiredColor == detectedColor) {
		servoLeft.write(90);
		servoRight.write(90);
		updatePIDValues();
		AsyncHandler.removeCallback(&updatePIDValues);
		CanIntake::beginCollection(requiredColor);
	}
	else {
		AsyncHandler.addCallback(&checkColorSensor);
	}
}

void MotorDrive::updatePIDValues() {
	//Serial.print("every100millis");
	addLeftPidValues();
	addRightPidValues();
	addLinePidValues(dir);
	servoLeft.write(constrain(throttleLeft, 0, 180) * dir);
	servoRight.write(constrain(throttleRight, 0, 180) * dir);
	AsyncHandler.addCallback(&updatePIDValues, 100);
}


void MotorDrive::addLeftPidValues() {
	leftPID.run();
	//Serial.print(outputLDrive);
	throttleLeft += outputLDrive;
}
void MotorDrive::addRightPidValues() {
	rightPID.run();
	//Serial.print(outputRDrive);
	throttleRight += outputRDrive;
}

//implements line following PID and adds values to drive
void MotorDrive::addLinePidValues(int dir) {
	irLeftAddition = 0;
	irRightAddition = 0;
	if (dir == 1) {
		//front sensor
		inputIR = analogRead(A0);
	}
	else {
		//rear sensor
		inputIR = analogRead(A1);
	}
	Serial.print(inputIR);
	setPointIR = 2.5;
	sensorPID.run();
	double out = outputFromIR;
	if (out > 2.5) {
		irRightAddition = out;
	}
	else if (out < 2.5) {
		irLeftAddition = out;
	}
	throttleLeft += irLeftAddition;
	throttleRight += irRightAddition;
}

void MotorDrive::requestColor(CanType canColor) {
	requiredColor = canColor;
}