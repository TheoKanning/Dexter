#include <TimerOne.h>
#include <TimerThree.h>

const int maxSpeed = 400; // steps/sec

const int leftStepPin  = 15; // B6
const int leftDirPin   = 14; // B5
const int rightStepPin = 13; // B4
const int rightDirPin  = 12; // D7

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

void setLeftSpeed(int stepsPerSecond) {
  stepsPerSecond = min(max(stepsPerSecond, -maxSpeed), maxSpeed);
  Timer1.setPeriod(1000000/abs(stepsPerSecond));
  setDirection(leftDirPin, stepsPerSecond);
}

void setRightSpeed(int stepsPerSecond) {
  stepsPerSecond = min(max(stepsPerSecond, -maxSpeed), maxSpeed);
  Timer3.setPeriod(1000000/abs(stepsPerSecond));
  setDirection(rightDirPin, stepsPerSecond);
}

void setDirection(int pin, int stepsPerSecond) {
  if (stepsPerSecond > 0) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}
