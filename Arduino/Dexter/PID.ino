
long lastUpdateMicroSec = micros();
float lastError = 0; 

float calculateSpeed(float angle, float setPoint, float Kp, float Kd) {
  float error = setPoint - angle;

//  float derivError = (error - lastError) / (micros() - lastUpdateMicroSec);
//  lastUpdateMicroSec = micros();
//  lastError = error;
  float steps = error * Kp;// + derivError * Kd;
//  Serial.print("Error: ");
//  Serial.print(error);
//  Serial.print(" Kp: ");
//  Serial.print(Kp);
//  Serial.print("Steps: ");
//  Serial.println(steps);
  return steps;
}
