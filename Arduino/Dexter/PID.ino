float speedIntError = 0;
float lastSpeedError = 0;

float lastAngleError = 0;

float speedPid(float speed, float setSpeed) {
  float error = setSpeed - speed;
  int maxAngle = 5;
  speedIntError += constrain(error / FREQUENCY, -10, 10);
  speedIntError = constrain(speedIntError, -maxAngle/speedKi, maxAngle/speedKi);
  float derivError = (error - lastSpeedError) * FREQUENCY;
  lastSpeedError = error;

  float angle = -(speedKp * error + speedKi * speedIntError + speedKd * derivError);
  angle = constrain(angle, -maxAngle, maxAngle);

  #if LOG_SPEED_PID
    LOG.print("Set Speed: ");
    LOG.print(setSpeed);
    LOG.print(" speed: ");
    LOG.print(speed / 20);
//    LOG.print(" Error: ");
//    LOG.print(error);
    LOG.print(" PropAngle: ");
    LOG.print(error * speedKp);
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
    LOG.print("Angle: ");
    LOG.print(angle);
    LOG.print(" SetAngle: ");
    LOG.print(setAngle);
    
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

enum TwiddleState {
  NONE,
  FORWARD,
  PAUSING,
  BACKWARD,
  SETTLING
};

TwiddleState twiddleState = NONE;
long lastTwiddleTime = millis();
long twiddleError = 0;
int previousSpeedCommand = 0;
int acceleration = 1;

void startTwiddling() {
  twiddleState = FORWARD;
  lastTwiddleTime = millis();
  twiddleError = 0;
  TUNE_SERIAL.println("Moving forward...");
}

/**
 * Checks twiddle state and returns desired speed
 */
float twiddle(int speed, float angle) {
  int speedCommand = 0;
  if (twiddleState == NONE) {
    return 0;
  }

  if (twiddleState == FORWARD) {
    speedCommand = 30;
    if (lastTwiddleTime + 3000 < millis()) {
      twiddleState = PAUSING;
      lastTwiddleTime = millis();
      TUNE_SERIAL.println("Pausing...");
    }
  }

  if (twiddleState == PAUSING) {
    speedCommand = 0;
    if (lastTwiddleTime + 5000 < millis()) {
      twiddleState = BACKWARD;
      lastTwiddleTime = millis();
      TUNE_SERIAL.println("Moving backward...");
    }
  }

  if (twiddleState == BACKWARD) {
    speedCommand = -30;
    if (lastTwiddleTime + 3000 < millis()) {
      twiddleState = SETTLING;
      lastTwiddleTime = millis();
      TUNE_SERIAL.println("Settling...");
    }
  }

  if (twiddleState == SETTLING) {
    speedCommand = 0;
    if (lastTwiddleTime + 5000 < millis()) {
      twiddleState = NONE;
      TUNE_SERIAL.print("Done! Error = ");
      TUNE_SERIAL.println(twiddleError);
    }
  }

  speedCommand = constrain(speedCommand, previousSpeedCommand - acceleration, previousSpeedCommand + acceleration);
  previousSpeedCommand = speedCommand;
  twiddleError += abs(speedCommand - speed);
  return speedCommand;
}
