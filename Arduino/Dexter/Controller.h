#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Arduino.h"

class MotorSpeed {
  public:
    MotorSpeed(double, double);
    double left;
    double right;
};

class Controller {
  public: 
    Controller();
    double roll = 0;
    double pitch = 0;
    double yaw = 0;
    MotorSpeed calculateMotorSpeeds(double);

  private:
    double lastError = 0;
    double lastErrorTime = 0;
    double integralError = 0;
    double integralMaxWindup = 1000;
    double Kp = 0.1;
    double Ki = 0;
    double Kd = 0.001;
};

#endif
