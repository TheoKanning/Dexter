#include "Arduino.h"
#include "Controller.h"

MotorSpeed::MotorSpeed(double left, double right) {
  this->left = left;
  this->right = right;
}

Controller::Controller(){
  this->lastErrorTime = millis();
};

MotorSpeed Controller::calculateMotorSpeeds(double angle) {
  double error = angle - pitch;

  this->integralError = constrain(this->integralError + error, -this->integralMaxWindup, this->integralMaxWindup);

  long deltaMs = millis() - this->lastErrorTime;
  double derivError = (error - this->lastError) / (deltaMs / 1000.0);
  this->lastError = error;
  this->lastErrorTime = millis();
  
  double action = this->Kp * error + this->Ki * this->integralError + this->Kd * derivError;
  
  Serial.print("Error: ");
  Serial.print(error);
  Serial.print(" Action: ");
  Serial.println(action);
  return MotorSpeed(action, action);
}
