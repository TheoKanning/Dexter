#include <TimerOne.h>
#include <TimerThree.h>

#define TIMER_FREQ 1000000

const int microsteps = 8; // A4988 is set for 1/8th steps

const int microstepPin1 = 6;
const int microstepPin2 = 7;
const int microstepPin3 = 8;
const int rightDirPin  = 20;
const int rightStepPin = 21;
const int leftDirPin   = 22;
const int leftStepPin  = 23;


void leftStep() {
  digitalWrite(leftStepPin, HIGH);
  digitalWrite(leftStepPin, LOW);
}

void rightStep() {
  digitalWrite(rightStepPin, HIGH);
  digitalWrite(rightStepPin, LOW);
}

void enableSteppers() {
  pinMode(microstepPin1, OUTPUT);
  pinMode(microstepPin2, OUTPUT);
  pinMode(microstepPin3, OUTPUT);
  
  pinMode(rightDirPin, OUTPUT);
  pinMode(rightStepPin, OUTPUT);
  pinMode(leftDirPin, OUTPUT);
  pinMode(leftStepPin, OUTPUT);
  
  Timer1.initialize(TIMER_FREQ);
  Timer1.attachInterrupt(leftStep);
  Timer3.initialize(TIMER_FREQ);
  Timer3.attachInterrupt(rightStep);

  setMicrosteps(microsteps);
}

void setLeftSpeed(double stepsPerSecond) {
  stepsPerSecond = constrainSteps(stepsPerSecond);
  Timer1.setPeriod(TIMER_FREQ / abs(stepsPerSecond * microsteps));
  setDirection(leftDirPin, stepsPerSecond);
}

void setRightSpeed(double stepsPerSecond) {
  stepsPerSecond = constrainSteps(stepsPerSecond);
  Timer3.setPeriod(TIMER_FREQ / abs(stepsPerSecond * microsteps));
  setDirection(rightDirPin, stepsPerSecond);
}

double constrainSteps(int stepsPerSecond) {
  // prevent instabilites at very low rates
  if (stepsPerSecond < 0 && stepsPerSecond > -1) {
    return -1;
  } else if (stepsPerSecond >= 0 && stepsPerSecond < 1) {
    return 1;
  }
  return constrain(stepsPerSecond, -MAX_SPEED, MAX_SPEED);
}

void setDirection(int pin, int stepsPerSecond) {
  if (stepsPerSecond >= 0) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}

void setMicrosteps(int microsteps) {
  switch(microsteps) {
    case 1:
      digitalWrite(microstepPin1, LOW);
      digitalWrite(microstepPin2, LOW);
      digitalWrite(microstepPin3, LOW);
      break;
    case 2:
      digitalWrite(microstepPin1, HIGH);
      digitalWrite(microstepPin2, LOW);
      digitalWrite(microstepPin3, LOW);
      break;
    case 4:
      digitalWrite(microstepPin1, LOW);
      digitalWrite(microstepPin2, HIGH);
      digitalWrite(microstepPin3, LOW);
      break;
    case 8:
      digitalWrite(microstepPin1, HIGH);
      digitalWrite(microstepPin2, HIGH);
      digitalWrite(microstepPin3, LOW);
      break;
    case 16:
      digitalWrite(microstepPin1, HIGH);
      digitalWrite(microstepPin2, HIGH);
      digitalWrite(microstepPin3, HIGH);
      break;
  }
}
