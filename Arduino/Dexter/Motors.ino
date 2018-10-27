#include <TimerOne.h>
#include <TimerThree.h>

const int maxSpeed = 500; // steps/sec
const int microsteps = 16; // A4988 is set for 1/16th steps

const int leftStepPin  = 23; // B6
const int leftDirPin   = 22; // B5
const int rightStepPin = 21; // B4
const int rightDirPin  = 20; // D7

int getMaxSpeed() {
  return maxSpeed;
}

void leftStep() {
  digitalWrite(leftStepPin, HIGH);
  digitalWrite(leftStepPin, LOW);
}

void rightStep() {
  digitalWrite(rightStepPin, HIGH);
  digitalWrite(rightStepPin, LOW);
}

void enableSteppers() {
  pinMode(rightStepPin, OUTPUT);
  pinMode(rightDirPin, OUTPUT);
  pinMode(leftStepPin, OUTPUT);
  pinMode(leftDirPin, OUTPUT);
  Timer1.initialize(100000);
  Timer1.attachInterrupt(leftStep);
  Timer3.initialize(100000);
  Timer3.attachInterrupt(rightStep);
}

void setLeftSpeed(double stepsPerSecond) {
  stepsPerSecond = min(max(stepsPerSecond, -maxSpeed), maxSpeed);
  Timer1.setPeriod(1000000/abs(stepsPerSecond * microsteps));
  setDirection(leftDirPin, -stepsPerSecond); // negative for left motor
}

void setRightSpeed(double stepsPerSecond) {
  stepsPerSecond = min(max(stepsPerSecond, -maxSpeed), maxSpeed);
  Timer3.setPeriod(1000000/abs(stepsPerSecond * microsteps));
  setDirection(rightDirPin, stepsPerSecond);
}

void setDirection(int pin, int stepsPerSecond) {
  if (stepsPerSecond > 0) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}
