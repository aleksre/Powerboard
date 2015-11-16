String serialString = "";         // a string to hold incoming data
boolean stringComplete = false;   // whether the string is complete


void initBluetooth() {
  serialString.reserve(100); 
}

void checkBtMessage() {
  if (stringComplete) {
    // Handle bluetooth message, then clear string
    handleBluetoothMessage(serialString);
    serialString = "";
    stringComplete = false;
  }
}

void handleBluetoothMessage(String msg) {
  Serial.println(msg);
  // Handle message
  if (msg.equals("connect")) {
    sendBoardStatus();
    sendBatteryInfo();
    sendTripInfo();
    showAlert(0, 0, 0, 0, 0, 255, 10);
    playConnectSE();
    showAlert(0, 0, 255, 0, 0, 0, 200);
  } else if (msg.equalsIgnoreCase("status=standby")) {
    switchMode(STANDBY);
  } else if (msg.equalsIgnoreCase("status=drive")) {
    switchMode(DRIVING);
  } else if (msg.equalsIgnoreCase("status=diagnostics")) {
    switchMode(DIAGNOSTICS);
  } else if (msg.equalsIgnoreCase("locked=true")) {
    switchMode(LOCKED);
    saveBoolean(EEPROM_LOCKED, true);
  } else if (msg.equalsIgnoreCase("locked=false")) {
    switchMode(STANDBY);
    saveBoolean(EEPROM_LOCKED, false); 
  } else if (msg.equalsIgnoreCase("standbyLights=1")) {
    showAlert(0, 0, 0, 0, 0, 255, 10);
    standbyLights = true;
    saveBoolean(EEPROM_STANDBY_LIGHTS, standbyLights);
    showAlert(0, 0, 255, 0, 0, 0, 200);
  } else if (msg.equalsIgnoreCase("standbyLights=0")) {
    showAlert(0, 0, 0, 0, 0, 255, 10);
    standbyLights = false;
    saveBoolean(EEPROM_STANDBY_LIGHTS, standbyLights);
    showAlert(0, 0, 255, 0, 0, 0, 200);
    lightsOff();
  } else if (msg.equalsIgnoreCase("drivingLights=1")) {
    showAlert(0, 0, 0, 0, 0, 255, 10);
    drivingLights = true;
    saveBoolean(EEPROM_DRIVING_LIGHTS, drivingLights);
    showAlert(0, 0, 255, 0, 0, 0, 200);
  } else if (msg.equalsIgnoreCase("drivingLights=0")) {
    showAlert(0, 0, 0, 0, 0, 255, 10);
    drivingLights = false;
    saveBoolean(EEPROM_DRIVING_LIGHTS, drivingLights);
    showAlert(0, 0, 255, 0, 0, 0, 200);
  } else if (msg.equalsIgnoreCase("endTrip=1")) { // ????
    showAlert(0, 0, 0, 0, 0, 255, 10);
    resetTrip();
    showAlert(0, 0, 255, 0, 0, 0, 200);
  } else if (msg.equalsIgnoreCase("calibrate=1")) {
    showAlert(0, 0, 0, 255, 0, 0, 10);
    mirrorLedSweep(0, 0, 0, 40, 0, 0, 255, 0, 0, 4, 1000);
    mirrorLedSweep(255, 0, 0, 40, 0, 0, 0, 0, 0, 0.25, 1000);
    runCalibration();
    playCalibrationSE();
    showAlert(255, 0, 0, 0, 0, 0, 200);
  } else if (msg.startsWith("weight=")) {
    if (mode == DIAGNOSTICS) {
      String data = msg.substring(7); // data = "xx,xx"
      int front = data.toInt();
      int back = data.substring(data.indexOf(",")+ 1).toInt(); 
      setVirtualWeight(front, back);
    }
  }
}

void sendBoardStatus() {
  Serial.println("model=Momentum Alpha");
  switchMode(mode); // <- Will print out current mode
}

void sendTripInfo() {
  Serial.print("currenttrip=");
  Serial.println(getCurrentTripInKmh());
  Serial.print("totaltrip=");
  Serial.println(getTotalTripInKmh());
}

void sendBatteryInfo() {
  // Todo
  Serial.println("battery=91");
}

void sendKmh() {
  Serial.print("kmh=");
  Serial.println(getSpeedInKmh());
}

void sendWeightSensorValues(int front, int back) {
  Serial.print("sensor=");
  Serial.print(front);
  Serial.print(",");
  Serial.println(back);
}

// Called automatically whenever there is serial data in the buffer
void serialEvent() { 
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      // Testing new bt handler:
      handleBluetoothMessage(serialString);
      serialString = "";
      stringComplete = false;
    } else {
      // add it to the serialString:
      serialString += inChar;
    }
  }
}
