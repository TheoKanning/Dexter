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

//  Serial.print("Set Speed: ");
//  Serial.print(setSpeed);
//  Serial.print(" speed: ");
//  Serial.print(speed);
//  Serial.print(" Error: ");
//  Serial.print(error);
//  Serial.print(" Kp: ");
//  Serial.print(speedKp, 4);
//  Serial.print(" Kd: ");
//  Serial.print(speedKd, 4);
//  Serial.print(" DerivSteps: ");
//  Serial.print(derivError * speedKd);
//  Serial.print(" Angle: ");
//  Serial.println(angle);
  return angle;
}

float anglePid(float angle, float setAngle) {
  float error = setAngle - (angle - angleOffset);

  float derivError = (error - lastAngleError) / (FREQUENCY);
  lastAngleError = error;
  
  float steps = error * angleKp + derivError * angleKd;
  steps = constrain(steps, -MAX_SPEED, MAX_SPEED);

//  if (setAngle == 0) {
//    if (steps > 0) {
//      angleOffset += 0.001;
//    } else if (steps < 0) {
//      angleOffset -= 0.001;
//    }
//  }
  
//  Serial.print("Error: ");
//  Serial.println(error);
//  Serial.print("Kp: ");
//  Serial.print(angleKp);
//  Serial.print(" Kd: ");
//  Serial.print(angleKd);
//  Serial.print(" DerivSteps: ");
//  Serial.println(derivError * Kd);
//  Serial.print(" Steps: ");
//  Serial.println(steps);
  return steps;
}
