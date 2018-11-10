float sumSpeedError = 0;
float lastSpeedError = 0;

float lastAngleError = 0;
float angleOffset = 0;

float speedPid(float speed, float setSpeed) {
  float error = setSpeed - speed;
  
  sumSpeedError += error;
  sumSpeedError = constrain(sumSpeedError, -100000, 100000);
  
  float derivError = (error - lastSpeedError) / (FREQUENCY);
  lastSpeedError = error;

  float angle = -(error * speedKp + sumSpeedError * 0.00001 + derivError * speedKd);
  angle = constrain(angle, -MAX_ANGLE, MAX_ANGLE);

  #if LOG_SPEED_PID
    LOG.print("Set Speed: ");
    LOG.print(setSpeed);
    LOG.print(" speed: ");
    LOG.print(speed);
    LOG.print(" Error: ");
    LOG.print(error);
    LOG.print(" Kp: ");
    LOG.print(speedKp, 4);
    LOG.print(" Kd: ");
    LOG.print(speedKd, 4);
    LOG.print(" DerivSteps: ");
    LOG.print(derivError * speedKd);
    LOG.print(" Angle: ");
    LOG.println(angle);
  #endif
  return angle;
}

float anglePid(float angle, float setAngle) {
  float error = setAngle - (angle - angleOffset);

  float derivError = (error - lastAngleError) * FREQUENCY;
  lastAngleError = error;
  
  float steps = error * angleKp + derivError * angleKd;
  
  steps = constrain(steps, -MAX_SPEED, MAX_SPEED);

  #if LOG_ANGLE_PID
    LOG.print("Error: ");
    LOG.print(error);
    LOG.print(" Kp: ");
    LOG.print(angleKp);
    LOG.print(" Kd: ");
    LOG.print(angleKd);
    LOG.print(" DerivSteps: ");
    LOG.print(derivError * angleKd);
    LOG.print(" Steps: ");
    LOG.print(steps);
    LOG.println("");
  #endif
  return steps;
}
