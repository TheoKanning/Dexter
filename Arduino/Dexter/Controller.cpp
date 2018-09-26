#include "Arduino.h"
#include "Controller.h"

MotorSpeed::MotorSpeed(double left, double right) {
  this->left = left;
  this->right = right;
}

Controller::Controller(){
  this->lastErrorTime = millis();
};

MotorSpeed Controller::calculateMotorSpeeds(double linear, double angular) {
  double error = -pitch;

  this->integralError += error;

  long deltaMs = millis() - this->lastErrorTime;
  double derivError = (error - this->lastError) / (deltaMs / 1000.0);
  this->lastError = error;
  this->lastErrorTime = millis();
  
  double action = this->Kp * error + this->Ki * this->integralError + this->Kd * derivError;  
  return MotorSpeed(action, action);
}
