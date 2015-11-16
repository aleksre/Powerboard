/* ESC */
#include <Adafruit_TiCoServo.h>
#include <known_16bit_timers.h>
#include <math.h>
const int ESC_MIN_MS = 600;
const int ESC_NEUTRAL_MS = 1500;
const int ESC_MAX_MS = 2400;
int throttleOffset = 220;           // Dynamic offset based on weight?
float throttleModifyer = 1.0;
const float MAX_MS_CHANGE = 2.0;    // Max change in ESC MS per update
boolean calibratingEsc = false;

Adafruit_TiCoServo esc;
int targetMs;
float currentMs = 1500.0;           // Saving speed as float for increased sub-ms accuracy when smoothing
float SMOOTHNESS = 0.1;

/* RPM and KM/H */
volatile int currentHalfRevs = 0;
volatile unsigned long tripHalfRevs = 0;
volatile unsigned long totalHalfRevs = 0;
int rpm = 0;
const double DIAMETER = 0.085;      // Wheel diameter in meters VARIABLE


void setMotorSpeed(float balance) {
  targetMs = getServoMs(balance);
  
  if (targetMs == ESC_NEUTRAL_MS) { // Neutral: Go to target instantly
    currentMs = targetMs;
  } else if (targetMs != round(currentMs)) { // Increment smoothly to target
    currentMs += constrain((targetMs - currentMs) * SMOOTHNESS, -MAX_MS_CHANGE, MAX_MS_CHANGE);
  }
  Serial.println(currentMs);
  esc.write(round(currentMs));
}

void setInstantMotorSpeed(float balance) {
  currentMs = getServoMs(balance);
  esc.write(currentMs);
}

int getServoMs(float balance) {
  // Calculate exponential throttle curve
  float exponential = pow(balance, 3);
  //if (balance < 0.0) exponential = -exponential; // Use if power of 2
  
  // Calculate min and max range limits, without offset
  int minimum = ESC_NEUTRAL_MS - ((ESC_NEUTRAL_MS - ESC_MIN_MS) * throttleModifyer - throttleOffset); //1500-680=820
  int maximum = ESC_NEUTRAL_MS + ((ESC_MAX_MS - ESC_NEUTRAL_MS) * throttleModifyer - throttleOffset); //1500+680=2180
  
  int milliSeconds = range(exponential, -1, 1, minimum, maximum);
  
  // Add offset (minimum speed) to limit cogging
  if (milliSeconds > ESC_NEUTRAL_MS) milliSeconds += throttleOffset;
  if (milliSeconds < ESC_NEUTRAL_MS) milliSeconds -= throttleOffset;
  return milliSeconds;
}

void arm(){
  esc.attach(9, ESC_MIN_MS, ESC_MAX_MS);
  setInstantMotorSpeed(0);
}

/* RPM & TRIP INFO */
void resetTrip() {
  noInterrupts();
  tripHalfRevs = 0;
  interrupts();
}

float getCurrentTripInKmh() {
  noInterrupts();
  float revolutions = (float) tripHalfRevs / 2;
  interrupts();
  return (revolutions * DIAMETER * PI) / 1000;
}

float getTotalTripInKmh() {
  noInterrupts();
  float revolutions = (float) totalHalfRevs / 2;
  interrupts();
  return (revolutions * DIAMETER * PI) / 1000;
}

float getSpeedInKmh() {
  // Calculate RPM, reset counter
  noInterrupts();
  rpm = currentHalfRevs * 15;
  currentHalfRevs = 0; 
  interrupts();
  
  float metersPerSecond = DIAMETER * rpm * 0.10472;
  return metersPerSecond * 3.6;
}

/*
void sendCurrentKmh() {
  detachInterrupt(0); //Disable interrupt when calculating
  rpm = currentHalfRevs * 15;
  double metersPerSecond = DIAMETER * rpm * 0.10472;
  double kmh = metersPerSecond * 3.6;
  Serial.print("kmh=");
  Serial.println(kmh);
  
  currentHalfRevs = 0; // Restart the RPM counter
  attachInterrupt(0, rpmCounter, CHANGE); // Enable interrupt
}*/

void saveTripInfoToLocalStorage() {
  noInterrupts();
  saveLong(EEPROM_CURRENT_TRIP, tripHalfRevs);
  saveLong(EEPROM_TOTAL_TRIP, totalHalfRevs);
  interrupts();
}

void startEscCalibration() {
  calibratingEsc = true;
  Serial.println("ESC Calibration");
  Serial.println("1 = Reverse, 2 = Neutral, 3 = Foreward, q = Quit Calibration.");
  while (calibratingEsc) {
    if (Serial.available()) {
      char inChar = (char)Serial.read();
      if (inChar == '1') {
        Serial.println("Setting motor speed: Reverse");
        esc.write(ESC_MIN_MS);
      } else if (inChar == '2') {
        Serial.println("Setting motor speed: Neutral");
        esc.write(ESC_NEUTRAL_MS);
      } else if (inChar == '3') {
        Serial.println("Setting motor speed: Foreward");
        esc.write(ESC_MAX_MS);
      } else if (inChar == 'q') {
        Serial.println("Quit Calibration");
        calibratingEsc = false;
      }
    }
  }
}

// Called automatically whenever PIN 2 changes between HIGH and LOW
void rpmCounter(){
  currentHalfRevs++;
  tripHalfRevs++;
  totalHalfRevs++;
  Serial.println("RPM");
}
