#include <SoftwareSerial.h>
#include <Adafruit_MotorShield.h>
#include <SerialCommand.h>
#include <AccelStepper.h>
#include "Controller.h"

#define LEFT_MOTOR_NUMBER  2
#define RIGHT_MOTOR_NUMBER 1
#define STEPS_PER_REVOLUTION 200
#define REVOLUTIONS_PER_MINUTE 120

#define BLUETOOTH_TIMEOUT 1000
#define FREQUENCY 20 // number of motor updates per second

SoftwareSerial btSerial(3, 2); //RX | TX pins
SoftwareSerial imuSerial(5, 4);

SerialCommand serialCommand(imuSerial);

Adafruit_MotorShield motorManager;
Adafruit_StepperMotor *motorLeft = motorManager.getStepper(STEPS_PER_REVOLUTION, LEFT_MOTOR_NUMBER);
Adafruit_StepperMotor *motorRight = motorManager.getStepper(STEPS_PER_REVOLUTION, RIGHT_MOTOR_NUMBER);
AccelStepper stepperLeft(leftForward, leftBackward);
AccelStepper stepperRight(rightForward, rightBackward);

Controller controller;

long lastUpdateTimeMs = millis();
int fallThreshold = 15; // give up if robot is more than this many degrees from vertical
boolean fallen = false;
float pitchCorrection = -0.87; // The pitch value when vertical, corrects construction errors
int count = 0;

void setup() {
  Serial.begin(57600);
  Serial.println("Dexter is starting...");
  //btSerial.begin(38400); //Baud rate may vary depending on your chip's settings!
  imuSerial.begin(38400);
  imuSerial.listen();
  serialCommand.addCommand("R", rollReceived);
  serialCommand.addCommand("P", pitchReceived);
  serialCommand.addCommand("Y", yawReceived);
  serialCommand.addDefaultHandler(unrecognized);
  initMotors();
}

void loop() {
  checkForFall(controller.pitch);
  
  if (fallen) {
    //releaseMotors();
  } else {
    stepperLeft.runSpeed();
    stepperRight.runSpeed();
  }
  count++;
  if (millis() - lastUpdateTimeMs < 1000 / FREQUENCY) {
    return;
  }
  lastUpdateTimeMs = millis();
  Serial.println(count);
  count = 0;
  serialCommand.readSerial();
  MotorSpeed speeds = controller.calculateMotorSpeeds(0);
  setMotorSpeeds(speeds.left, speeds.right);
}

void rollReceived() {
  double roll = wrapAngle(atof(serialCommand.next()));
  //logAngle("Roll", roll);
  controller.roll = roll;
}

void pitchReceived() {
  double pitch = wrapAngle(atof(serialCommand.next())) - pitchCorrection;
  //logAngle("Pitch", pitch);
  controller.pitch = 0.5 * controller.pitch + 0.5 * pitch;
}

void yawReceived() {
  double yaw = wrapAngle(atof(serialCommand.next()));
  //logAngle("Yaw", yaw);
  controller.yaw = yaw;
}

void unrecognized() {
  char *next = serialCommand.next();
  Serial.print("Unrecognized command: ");
  if (next != NULL) {
    Serial.println(next);
  } else {
    Serial.println("No second argument");
  }
}

void initMotors() {
  motorManager.begin();
  stepperLeft.setMaxSpeed(2 * STEPS_PER_REVOLUTION);
  stepperRight.setMaxSpeed(2 * STEPS_PER_REVOLUTION);
  stepperLeft.setAcceleration(0); // ignored if set to zero
  stepperRight.setAcceleration(0);
}

/*
* Sets all motors speeds, assumes good data
*/
void setMotorSpeeds(float leftMotorSpeed, float rightMotorSpeed) {
  // setting speed to 0 causes AccelStepper to ignore value and travel at max speed instead.
  if (leftMotorSpeed == 0) leftMotorSpeed = 0.001;
  if (rightMotorSpeed == 0) rightMotorSpeed = 0.001;
  stepperLeft.setSpeed(leftMotorSpeed * STEPS_PER_REVOLUTION);
  stepperRight.setSpeed((rightMotorSpeed * STEPS_PER_REVOLUTION));
  
  //Serial.print("Actual: ");
  //Serial.print(stepperLeft.speed());
  //Serial.print(" Expected: ");
  //Serial.print(leftMotorSpeed * STEPS_PER_REVOLUTION);
  //Serial.println(" steps/sec");
}

void releaseMotors(){
  motorLeft->release();
  motorRight->release();
}

void checkForFall(double pitch) {
  fallen = pitch > fallThreshold || pitch < - fallThreshold;
}

// adds and subtracts 360 degrees until angle is between -180 and 180
double wrapAngle(double angle) {
  while(angle > 180) {
    angle -= 360;
  }
  while(angle < -180) {
    angle += 360;
  }
  return angle;
}

void logAngle(char* name, double angle){
  Serial.print(name);
  Serial.print(": ");
  Serial.println(angle);
}

void leftForward() {
  motorLeft->onestep(FORWARD, DOUBLE);
}

void leftBackward() {
  motorLeft->onestep(BACKWARD, DOUBLE);
}

void rightForward() {
  // opposite direction because of motor mounting
  //motorRight->onestep(BACKWARD, DOUBLE);
}

void rightBackward() {
  // opposite direction because of motor mounting
  //motorRight->onestep(FORWARD, DOUBLE);
}
