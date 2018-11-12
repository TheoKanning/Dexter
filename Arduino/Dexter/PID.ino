float speedIntError = 0;

float lastAngleError = 0;

float speedPid(float speed, float setSpeed) {
  float error = setSpeed - speed;
  int maxAngle = 3;
  speedIntError += constrain(error / FREQUENCY, -10, 10);
  speedIntError = constrain(speedIntError, -maxAngle/speedKi, maxAngle/speedKi);

  float angle = -(speedKp * error + speedKi * speedIntError);
  angle = constrain(angle, -maxAngle, maxAngle);

  #if LOG_SPEED_PID
    LOG.print("Set Speed: ");
    LOG.print(setSpeed);
    LOG.print(" speed: ");
    LOG.print(speed);
    LOG.print(" Error: ");
    LOG.print(error);
    LOG.print(" Kp: ");
    LOG.print(speedKp, 4);
    LOG.print(" Ki: ");
    LOG.print(speedKi, 4);
    LOG.print(" IntAngle: ");
    LOG.print(- speedKi * speedIntError);
    LOG.print(" Angle: ");
    LOG.println(angle);
  #endif
  return angle;
}

float anglePid(float angle, float setAngle) {
  float error = setAngle - angle;
  float derivError = (error - lastAngleError) * FREQUENCY;
  lastAngleError = error;
  
  float steps = angleKp * error + angleKd * derivError;
  
  steps = constrain(steps, -MAX_SPEED, MAX_SPEED);

  #if LOG_ANGLE_PID
//    LOG.print("Error: ");
//    LOG.print(error);
//    LOG.print(" Kp: ");
//    LOG.print(angleKp);
//    LOG.print(" Kd: ");
//    LOG.print(angleKd);
    LOG.print(" PropSteps: ");
    LOG.print(angleKp * error);
    LOG.print(" DerivSteps: ");
    LOG.print(angleKd * derivError);
    LOG.print(" Steps: ");
    LOG.print(steps);
    LOG.println("");
  #endif
  return steps;
}
