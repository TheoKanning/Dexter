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
  
  long deltaMs = millis() - this->lastErrorTime;
  double error = angle - pitch;

  this->integralError += error * deltaMs;
  this->integralError = constrain(this->integralError , -this->integralMaxWindup, this->integralMaxWindup);
  
  double derivError = (error - this->lastError) / ( deltaMs / 1000.0) ;
  
  this->lastError = error;
  this->lastErrorTime = millis();
  
  double output = this->Kp * error + this->Ki * this->integralError + this->Kd * derivError;
  
  //Serial.print("Error: ");
  //Serial.print(error);
  //Serial.print(" Prop: ");
  //Serial.print(this->Kp * error);
  //Serial.print(" Deriv: ");
  //Serial.print(this->Kd * derivError * 200);
  //Serial.print(" Output: ");
  //Serial.println(output * 200);
  return MotorSpeed(output, output);
}
