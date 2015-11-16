/* Weight sensor */
int backInputPin = A0;
int frontInputPin = A1;
int frontSensorOffset = loadByte(EEPROM_FRONT_SENSOR);
int backSensorOffset = loadByte(EEPROM_BACK_SENSOR);

int virtualFrontSensor = 50;
int virtualBackSensor = 50;

/* Weight input smoothing */
const int NUMREADINGS = 10;         // More = Smoother, Less = Faster
int index = 0;                      // Index of current reading

int frontValues[NUMREADINGS];       // Analog front readings
int backValues[NUMREADINGS];        // Analog back readings
int frontTotal;                     // The total of all front values
int backTotal;                      // The total of all back values

/* Balance */
const float MAX_BALANCE = 0.85;      // Balance reaches maximum at 85%
const float FRONT_REDUCER = 0.22;   // Reduce front sensor value with n % of back sensor VARIABLE

void initLoadCell() {
  performMeasurement();
  for (int i = 0; i < NUMREADINGS; i++) {
    frontValues[i] = getFrontSensor();
    backValues[i] = getBackSensor();
  }
}

void performMeasurement() {
  int rawFrontValue = analogRead(frontInputPin);
  int rawBackValue = analogRead(backInputPin);
  // Subtract old reading then add new reading from sensor:
  frontTotal = frontTotal - frontValues[index];
  backTotal = backTotal - backValues[index];
  
  frontValues[index] = rawFrontValue - frontSensorOffset;
  backValues[index] = rawBackValue - backSensorOffset;
  
  frontTotal = frontTotal + frontValues[index];
  backTotal = backTotal + backValues[index];
  
  if (++index >= NUMREADINGS) index = 0; // Restart index
}

int getFrontSensor() {
  if (isPrototype2()) {
    if (frontTotal / NUMREADINGS > 0) {
      return frontTotal / NUMREADINGS;
    }
  } else {
    if ((frontTotal / NUMREADINGS) - (getBackSensor() * FRONT_REDUCER) > 0) {
      return (frontTotal / NUMREADINGS) - (getBackSensor() * FRONT_REDUCER);
    }
  }
  return 0;
}

int getBackSensor() {
  if (backTotal / NUMREADINGS > 0) {
    return backTotal / NUMREADINGS;
  }
  return 0;
}

float calculateBalance(int front, int back, float deadzone, float offset) {
  float balance = front - back; // Positive = Front, Negative = Back, 0 = Balanced
  float totalWeight = front + back;
  float result = balance / (totalWeight * MAX_BALANCE);
  
  if (result - offset < deadzone && result - offset > -deadzone) {
    return 0; 
  }
  if (result - offset < 0) {
    result = range(result, offset - deadzone, -1, 0, -1);
  } else {
    result = range(result, offset + deadzone, 1, 0, 1);
  }

  return constrain(result, -1, 1);

  // Return result: 1 = 100% front, -1 = 100% back, 0 = balanced
  if(result < 0) return range(result, -deadzone, -1, 0, -1);
  return range(result, deadzone, 1, 0, 1);
}

void runCalibration() {
  int maxFront = 0;
  int maxBack = 0;
  
  for (int i = 0; i < 100; i++) {
    int front = analogRead(frontInputPin);
    int back = analogRead(backInputPin);
    if (front > maxFront) maxFront = front;
    if (back > maxBack) maxBack = back;
    delay(1);
  }
  frontSensorOffset = maxFront;
  backSensorOffset = maxBack;
  
  saveByte(EEPROM_FRONT_SENSOR, frontSensorOffset);
  saveByte(EEPROM_BACK_SENSOR, backSensorOffset);
  Serial.print("calibrate=");
  Serial.print(frontSensorOffset);
  Serial.print(",");
  Serial.println(backSensorOffset);
}

void setVirtualWeight(int front, int back) {
  virtualFrontSensor = front;
  virtualBackSensor = back;
}

int getVirtualFrontSensor() {
  return virtualFrontSensor;
}

int getVirtualBackSensor() {
  return virtualBackSensor;
}

