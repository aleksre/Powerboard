/*
 * Copyright 2015 Aleksander Rem
 * Software for controlling a PMD using two load cells
 * Can control a brushless DC motor controller (thorugh a servo), LED strips, piezo speaker and bluetooth serial.
 */

#include "EEPROM_addresses.h"

const boolean prototype2 = false;

/* Modes */
const int STANDBY = 0;
const int DRIVING = 1;
const int DIAGNOSTICS = 2;
const int LOCKED = 3;
int mode = STANDBY;

/* Standby */
unsigned long activationMillis, deactivationMillis, kmhMillis, updateMillis, diagnosticsSensorMillis = millis();
const int MIN_WEIGHT = 100; // Must be checked while turning
const float STANDBY_DEADZONE = 0.30; // Activation deadzone percentage
const float DRIVING_DEADZONE = 0.10; // Driving mode balanced deadzone percentage
float balanceOffset = 0.0; // Save when initiating driving mode. This offset will then set the new "balanced" position.

boolean drivingLights = loadBoolean(EEPROM_DRIVING_LIGHTS);
boolean standbyLights = loadBoolean(EEPROM_STANDBY_LIGHTS);


void setup() {
  //Initialize serial, InteruptPIN, ESC and Input smoothing:
  Serial.begin(9600);
  attachInterrupt(0, rpmCounter, CHANGE); // Interrupt 0 is PIN 2.
  pinMode(2, INPUT_PULLUP); // Turn on internal pull-up (No 10k resistor needed)

  arm();
  initBluetooth();
  initLoadCell();
  initLeds();
  
  if (loadBoolean(EEPROM_LOCKED)) {
    switchMode(LOCKED);
  } else {
    mirrorLedSweep(0, 0, 0, 100, 140, 210, 20, 80, 200, 4, 900);
    mirrorLedSweep(20, 80, 200, 5, 20, 50, 0, 0, 0, 0.25, 900);
  }
}

void loop() {
  
  // Get Weight Input
  performMeasurement();
  int front = getFrontSensor();
  int back = getBackSensor();

  // Mode spesific actions
  if (mode == STANDBY) {
    standby(front, back);
  } else if (mode == DRIVING) {
    driving(front, back);
  } else if (mode == DIAGNOSTICS) {
    diagnostics(front, back);
  } else if (mode == LOCKED) {
    locked();
  }
  
  if (millis() - updateMillis >= 20000) {
    sendTripInfo();
    sendBatteryInfo();
    
    saveTripInfoToLocalStorage();
    updateMillis = millis();
  }
  
  delay(10); // Update 100 times per second
}

void standby(int front, int back) {
  Serial.print(front);
  Serial.print(" - ");
  Serial.println(back);
  setInstantMotorSpeed(0);
  if (front > MIN_WEIGHT || back > MIN_WEIGHT) { // Weight is applied

    //Serial.println(calculateBalance(front, back, STANDBY_DEADZONE, 0));
    if (calculateBalance(front, back, STANDBY_DEADZONE, 0) == 0) { // Balanced
      if (standbyLights) {
        if (millis() - activationMillis > 500) {
          activationLights(millis() - activationMillis - 500);
        } else {
          showBalanceLights(calculateBalance(front, back, STANDBY_DEADZONE, 0));
        }
      }
    } else { // Imbalanced
      if (standbyLights) showBalanceLights(calculateBalance(front, back, STANDBY_DEADZONE, 0));
      activationMillis = millis();
    }
    
    if (millis() - activationMillis > 800) {
      activate(front, back);
    }
  } else {
    // No weight is applied. Lights are dimmed
    lightsOff();
    activationMillis = millis();
  }
}

void activate(int front, int back) {
  if (standbyLights) showColor(0, 255, 0);
    
  // Save the current balance position as offset: 2 measurements
  balanceOffset = calculateBalance(front, back, 0, 0);
  switchMode(DRIVING);
  performMeasurement();
  balanceOffset = (balanceOffset + calculateBalance(getFrontSensor(), getBackSensor(), 0, 0)) / 2;
  
  if (standbyLights) {
    if (drivingLights) {
      sweepToDrivingMode(0.25, 500);
    } else {
      mirrorLedSweep(0, 255, 0, 0, 80, 0, 0, 0, 0, 0.25, 500);
      lightsOff();
    }
  }
}

void driving(int front, int back) {
  // Todo: Driving lights
  if (front > MIN_WEIGHT || back > MIN_WEIGHT) {
    if (drivingLights) showDrivingLights(calculateBalance(front, back, 0.05, balanceOffset));
    setMotorSpeed(calculateBalance(front, back, DRIVING_DEADZONE, balanceOffset));
    deactivationMillis = millis(); 
  } else {
    setInstantMotorSpeed(0); 
    if(millis() - deactivationMillis > 250) {
      // Todo: BREAK if speed is over 3 km/h
      switchMode(STANDBY);
      playDeactivateSE();
      lightsOff();
    }
  }
  if (millis() - kmhMillis >= 500) { 
    sendKmh();
    kmhMillis = millis();
  }
}

void diagnostics(int front, int back) {
  setMotorSpeed(calculateBalance(getVirtualFrontSensor(), getVirtualBackSensor(), 0.1, 0));
  if (millis() > diagnosticsSensorMillis + 100) {
    sendWeightSensorValues(front, back);
    diagnosticsSensorMillis = millis();
  }
}

void locked() {
  setInstantMotorSpeed(0);
  showColor(100, 0, 0);
}

void switchMode(int newMode) {
  if (newMode == STANDBY) {
    activationMillis = millis();
    Serial.println("status=Standby");
  } else if (newMode == DRIVING) {
    Serial.println("status=Drive");
    //halfRevolutions = 0; // Need?
    playActivateSE();
  } else if (newMode == DIAGNOSTICS) {
    Serial.println("status=Diagnostics");
    mirrorLedSweep(0,0,0,80,80,0,255,255,0, 4, 1000);
    mirrorLedSweep(255,255,0,80,80,0,0,0,0, 0.25, 1000);
    lightsOff();
  } else if (newMode == LOCKED) {
    Serial.println("status=Locked");
    mirrorLedSweep(0,0,0,100,40,40,100,0,0, 4, 1000);
  }
  setVirtualWeight(50, 50); // Reset virtual sensor regardless of mode switch
  mode = newMode;
}

float range(float value, float fromMin, float fromMax, float toMin, float toMax) {
  float a = value - fromMin;
  float b = toMax - toMin;
  float c = fromMax - fromMin;
  return a * b / c + toMin;
}

boolean isPrototype2() {
  return prototype2;
}

