#include "motor_drive.h"
#include "async_handler.h"
#include "can_intake.h"


double MotorDrive::countLeft = 0;
double MotorDrive::countRight = 0;
//
int MotorDrive::dir = 1;

int MotorDrive::targetDistance = 1000; // in revolutions

double MotorDrive::irRightAddition, MotorDrive::irLeftAddition, MotorDrive::driveLeftPidOutput, MotorDrive::driveRightPidOutput;
double MotorDrive::inputIR, MotorDrive::setPointIR, MotorDrive::outputFromIR;

//DRIVE PIDS
double MotorDrive::driveSetPoint;


bool MotorDrive::is_driving = false;
bool MotorDrive::trip_in_progress = false;


//rgb stuff
int MotorDrive::frequency, MotorDrive::redFreq, MotorDrive::greenFreq, MotorDrive::blueFreq;

// servos
Servo MotorDrive::servoLeft;
Servo MotorDrive::servoRight;

int MotorDrive::throttleLeft, MotorDrive::throttleRight;

bool MotorDrive::color_reading_in_progress;
millis_t MotorDrive::disable_color_sensor_until;

void MotorDrive::init() {
	pinMode(Pins::drive_door_solenoid, OUTPUT); // setup the solenoid
	pinMode(Pins::drive_door_switch, OUTPUT); // switch for something
	driveSetPoint = targetDistance;
	attachInterrupt(digitalPinToInterrupt(Pins::drive_ir_leftInterrupt), ISRleft, RISING);
	attachInterrupt(digitalPinToInterrupt(Pins::drive_ir_rightInterrupt), ISRright, RISING);
	pinMode(Pins::drive_ir_leftSensor, INPUT); // 18 and 22 come from left
	pinMode(Pins::drive_ir_rightSensor, INPUT); // 19 and 23 come from right
	pinMode(Pins::drive_rgb_s3, INPUT);//pinMode(6, INPUT); //enable pin
	servoLeft.attach(Pins::drive_motor_left);  // attaches the servo on pin 9 to the servo object
	servoRight.attach(Pins::drive_motor_right);  // attaches the servo on pin 9 to the servo object
	digitalWrite(Pins::drive_door_solenoid, LOW); // lock the door
}

bool MotorDrive::isDriving() noexcept {
	return is_driving;
}

bool MotorDrive::tripInProgress() noexcept {
	return trip_in_progress;
}

void MotorDrive::ISRleft() {
	if (digitalRead(Pins::drive_ir_leftSensor) == LOW) {
		countLeft = countLeft + 1;
	}
	else {
		countLeft = countLeft - 1;
	};
}
void MotorDrive::ISRright() {
	if (digitalRead(Pins::drive_ir_rightSensor) == HIGH) {
		countRight = countRight + 1;
	}
	else {
		countRight = countRight - 1;
	};
}

bool MotorDrive::inRange(int val, int minimum, int maximum) {
	return ((minimum <= val) && (val <= maximum));
}

void MotorDrive::driveSomewhere() {
	trip_in_progress = true;
	if (CanIntake::needsMoreCans()) {
		dir = 1;
		AsyncHandler.addCallback(checkColorSensor);
	}
	else {
		dir = -1;
	};
	// TODO: determine a distance to drive before stopping again if we need more cans from the
	// ... same color shelf but have exhausted the last row
	// ... (apparently about 1000 units of revolution)
	AsyncHandler.addCallback(driveLoop, 100);
}

void monitorDoorSwitchState() {
	if (digitalRead(Pins::drive_door_switch) == LOW) {
		digitalWrite(Pins::drive_door_solenoid, LOW);
	}
	else {
		AsyncHandler.addCallback(monitorDoorSwitchState);
	}
}

void MotorDrive::driveLoop() {
	if (CanIntake::needsMoreCans() || (countLeft <= 0 || countRight <= 0)) {
		is_driving = true;
		addLinePidValues();
		AsyncHandler.addCallback(driveLoop, 100);
	}
	else {
		stopMovement();
		digitalWrite(Pins::drive_door_solenoid, HIGH); // open can holder door
		digitalWrite(Pins::drive_door_switch, HIGH);
		AsyncHandler.addCallback(monitorDoorSwitchState, 10);
		//TODO: figure out control transition from here
		// door takes about 30? seconds to come back up to closed
		trip_in_progress = false;
	};
}



void MotorDrive::checkColorSensor() {
	if (color_reading_in_progress) return;
	millis_t time_now = millis(); // store to avoid overflow if the times are close
	if (time_now > disable_color_sensor_until) {
		color_reading_in_progress = true;
		checkColorSensorPhase1();
	}
	else {
		AsyncHandler.addCallback(checkColorSensor, disable_color_sensor_until - time_now);
	};

}

void MotorDrive::checkColorSensorPhase1() {
	digitalWrite(Pins::drive_rgb_s2, LOW);
	digitalWrite(Pins::drive_rgb_s3, LOW);
	redFreq = pulseIn(Pins::drive_rgb_sensorOut, LOW);
	redFreq = map(redFreq, 130, 555, 0, 255);
	AsyncHandler.addCallback(checkColorSensorPhase2, 100);
}

void MotorDrive::checkColorSensorPhase2() {
	digitalWrite(Pins::drive_rgb_s2, HIGH);
	digitalWrite(Pins::drive_rgb_s3, HIGH);
	greenFreq = pulseIn(Pins::drive_rgb_sensorOut, LOW);
	greenFreq = map(greenFreq, 190, 1300, 0, 255);
	AsyncHandler.addCallback(checkColorSensorPhase3, 100);
}

void MotorDrive::checkColorSensorPhase3() {
	digitalWrite(Pins::drive_rgb_s2, LOW);
	digitalWrite(Pins::drive_rgb_s3, HIGH);
	blueFreq = pulseIn(Pins::drive_rgb_sensorOut, LOW);
	blueFreq = map(blueFreq, 55, 370, 0, 255);
	color_reading_in_progress = false;
	disable_color_sensor_until = millis() + 100;
	AsyncHandler.addCallback(checkSensedColor);
}

void MotorDrive::checkSensedColor() {
	CanType detectedColor;
	if (blueFreq < 50 && blueFreq < greenFreq && blueFreq < redFreq) {
		detectedColor = CanType::blue;
	}
	else if (greenFreq < 50 && greenFreq < redFreq && greenFreq < blueFreq) {
		detectedColor = CanType::green;
	}
	else if (redFreq < 50 && redFreq < greenFreq && redFreq < blueFreq) {
		detectedColor = CanType::red;
	}
	else {
		AsyncHandler.addCallback(checkColorSensor, 100);
		return;
	};
	// we know we've read a color if we get here
	if (CanIntake::needsMoreCans(detectedColor)) {
		stopMovement();
		CanIntake::beginCollection(detectedColor);
	};
}


void MotorDrive::stopMovement() {
	is_driving = false;
	servoLeft.write(90);
	servoRight.write(90);
	addLinePidValues();
	AsyncHandler.removeCallback(driveLoop);
}
//IR PID consts


//implements line following PID and adds values to drive
void MotorDrive::addLinePidValues() {
	static constexpr int whiteTapeFwd = 563, whiteTapeRwd = 570;
	static constexpr int blackTapeFwd = 981, blackTapeRwd = 970;
	static constexpr double iZone = 10, kP = 0.02, kI = 0.0, kD = 0.0;
	static double previousError = 0;

	//int targetPosition = (blackTape + whiteTape) / 2;
	int targetPosition, currentPosition;
	if (dir == 1) { //checks which sensor to read and prepares the mapping values accordingly
		currentPosition = analogRead(Pins::drive_forward_sensor);
		targetPosition = (blackTapeFwd + whiteTapeFwd) / 2;
	}
	else {
		currentPosition = analogRead(Pins::drive_reverse_sensor);
		targetPosition = (blackTapeRwd + whiteTapeRwd) / 2;
	};
	int error = targetPosition - currentPosition;

	double integral = 0;
	if (abs(error) < iZone) {
		integral = (error * kI) + integral;
	}
	else {
		integral = 0;
	}

	double derivative = (error - previousError) * kD;
	previousError = error;

	int outputSpeed = error * kP + integral + derivative;

	int leftBaseSpeed, rightBaseSpeed;
	int leftWrite, rightWrite;
	if (dir == 1) { //moving forwards
		leftBaseSpeed = 75;
		rightBaseSpeed = 105;
		leftWrite = constrain(leftBaseSpeed - outputSpeed, 0, 80);
		rightWrite = constrain(rightBaseSpeed - outputSpeed, 100, 180);
	}
	else {
		leftBaseSpeed = 112;
		rightBaseSpeed = 75;
		leftWrite = constrain(leftBaseSpeed + outputSpeed, 100, 180);
		rightWrite = constrain(rightBaseSpeed + outputSpeed, 0, 80);
	}

	Serial.print(leftWrite);
	Serial.print("\t");
	Serial.print(rightWrite);
	servoLeft.write(leftWrite);
	servoRight.write(rightWrite);
	Serial.print("\n");
}