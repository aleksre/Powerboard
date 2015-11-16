#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define PIN                6
#define NUMPIXELS          22
#define NUMHEADLIGHTPIXELS 3
#define ID_OFFSET          -6

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

unsigned long startTime, endTime;
//double percent = 0.0;


void initLeds() {
  pixels.begin();
  pixels.show();
}

// No transition color
void ledSweep(int fromR, int fromG, int fromB, int toR, int toG, int toB, double powerOf, int milliSeconds) {
  startTime = millis();
  endTime = startTime + milliSeconds;

  float percent = 0.0;
  while(percent <= 1.0) {
    double progress = percent * NUMPIXELS;
    for (int i = NUMPIXELS; i > 0; i--) {
      int ledId = getLedId(i);
      
      if (NUMPIXELS - i <= progress - 1) { // After sweep: Full 'to' color
        pixels.setPixelColor(ledId, pixels.Color(toR, toG, toB));
      } else if (NUMPIXELS - i < progress) { // Transition color
        double ledPercentage = (progress - (NUMPIXELS - i)); // From 0 - 1
        pixels.setPixelColor(ledId, pixels.Color((toR - fromR) * ledPercentage + fromR, (toG - fromG) * ledPercentage + fromG, (toB - fromB) * ledPercentage + fromB));
      } else { // Before sweep: Full 'from' color
        pixels.setPixelColor(ledId, pixels.Color(fromR, fromG, fromB));
      }
      
    }
    pixels.show();
    delay(5);
    
    double linear = (double) (millis() - startTime) / (double) (endTime - startTime);
    percent = pow(linear, powerOf);
  }
}

// With transition color:
void ledSweep(int fromR, int fromG, int fromB, int midR, int midG, int midB, int toR, int toG, int toB, double powerOf, int milliSeconds) {
  startTime = millis();
  endTime = startTime + milliSeconds;

  float percent = 0.0;
  while(percent <= 1.0) {
    double progress = percent * (NUMPIXELS + 1);
    for (int i = NUMPIXELS; i > 0; i--) {
      int ledId = getLedId(i); 
      
      if (NUMPIXELS - i <= progress - 2) { // After sweep: To
        pixels.setPixelColor(ledId, pixels.Color(toR, toG, toB));
      } else if (NUMPIXELS - i < progress - 1) { // In sweep: To > Mid
        double ledPercentage = (progress - (NUMPIXELS - i + 1)); // From 0 - 1
        pixels.setPixelColor(ledId, pixels.Color((toR - midR) * ledPercentage + midR, (toG - midG) * ledPercentage + midG, (toB - midB) * ledPercentage + midB));
      } else if (NUMPIXELS - i < progress) { // In sweep: Mid > From
        double ledPercentage = (progress - (NUMPIXELS - i)); // From 0 - 1
        pixels.setPixelColor(ledId, pixels.Color((midR - fromR) * ledPercentage + fromR, (midG - fromG) * ledPercentage + fromG, (midB - fromB) * ledPercentage + fromB));
      } else { // Before sweep: From
        pixels.setPixelColor(ledId, pixels.Color(fromR, fromG, fromB));
      }
    }
    pixels.show();
    delay(5);
    
    double linear = (double) (millis() - startTime) / (double) (endTime - startTime);
    percent = pow(linear, powerOf);
  }
}

void mirrorLedSweep(int fromR, int fromG, int fromB, int midR, int midG, int midB, int toR, int toG, int toB, double powerOf, int milliSeconds) {
  startTime = millis();
  
  float percent = 0.0;
  while(percent <= 1.0) {
    double progress = percent * (NUMPIXELS / 2 + 2);
    for (int i = 0; i < NUMPIXELS / 2 + 1; i++) {
      int leftId = getLeftLedId(i);
      int rightId = getRightLedId(i);
      
      if (i < progress - 2) { // After sweep: To
        pixels.setPixelColor(leftId, pixels.Color(toR, toG, toB));
        pixels.setPixelColor(rightId, pixels.Color(toR, toG, toB));
      } else if (i < progress - 1) { // In sweep: To > Mid
        double ledPercentage = (progress - i - 1); // From 0 - 1
        uint32_t color = pixels.Color((toR - midR) * ledPercentage + midR, (toG - midG) * ledPercentage + midG, (toB - midB) * ledPercentage + midB);
        pixels.setPixelColor(leftId, color);
        pixels.setPixelColor(rightId, color);
      } else if (i < progress) { // In sweep: Mid > From
        double ledPercentage = (progress - i); // From 0 - 1
        uint32_t color = pixels.Color((midR - fromR) * ledPercentage + fromR, (midG - fromG) * ledPercentage + fromG, (midB - fromB) * ledPercentage + fromB);
        pixels.setPixelColor(leftId, color);
        pixels.setPixelColor(rightId, color);
      } else { // Before sweep: From
        pixels.setPixelColor(leftId, pixels.Color(fromR, fromG, fromB));
        pixels.setPixelColor(rightId, pixels.Color(fromR, fromG, fromB));
      }
    }
    pixels.show();
    delay(5);
    
    double linear = (double) (millis() - startTime) / (double) milliSeconds;
    percent = pow(linear, powerOf);
  }
}

// Unused
void inverseLedSweep(int fromR, int fromG, int fromB, int toR, int toG, int toB, double powerOf, int milliSeconds) {
  startTime = millis();
  endTime = startTime + milliSeconds;

  float percent = 0.0;
  while(percent <= 1.0) {
    double progress = percent * NUMPIXELS;
    for (int i = 0; i<NUMPIXELS; i++) {
      int ledId = getLedId(i); 
      
      if (i <= progress - 1.0) { // After sweep: Full 'to' color
        pixels.setPixelColor(ledId, pixels.Color(toR, toG, toB));
      } else if (i < progress) { // Transition color
        double ledPercentage = (progress - i); // From 0 - 1
        pixels.setPixelColor(ledId, pixels.Color((toR - fromR) * ledPercentage + fromR, (toG - fromG) * ledPercentage + fromG, (toB - fromB) * ledPercentage + fromB));
      } else { // Before sweep: Full 'from' color
        pixels.setPixelColor(ledId, pixels.Color(fromR, fromG, fromB));
      }
    }
    pixels.show();
    delay(5);
    
    double linear = (double) (millis() - startTime) / (double) (endTime - startTime);
    percent = pow(linear, powerOf);
  }
}

void activationLights(unsigned long ms) {
  float progress = pow((float) ms / 300, 3) * (NUMPIXELS / 4 + 1);
  int mirrorId = NUMPIXELS / 2 - 1;
  int frontId = NUMPIXELS / 2;
  for (int i = 0; i < NUMPIXELS / 4 + 1; i++) {
    
    if (i <= progress - 2) { // Full green
      uint32_t color = pixels.Color(0, 255, 0);
      pixels.setPixelColor(i, color);
      pixels.setPixelColor(mirrorId - i, color);
      pixels.setPixelColor(frontId + i, color);
      pixels.setPixelColor(frontId + mirrorId - i, color);
    } else if (i <= progress - 1) { // White-to-green
      float ledPercentage = progress - i - 1;
      uint32_t color = pixels.Color(80 - (80 * ledPercentage), 80 + (175 * ledPercentage), 255 - (255 * ledPercentage));
      pixels.setPixelColor(i, color);
      pixels.setPixelColor(mirrorId - i, color);
      pixels.setPixelColor(frontId + i, color);
      pixels.setPixelColor(frontId + mirrorId - i, color);
    } else if (i < progress) { // Off-to-green
      float ledPercentage = progress - i;
      uint32_t color;
      if (i == 0) { // Special case when going from balance lights to white activation lights
        color = pixels.Color(40 + (40 * ledPercentage), 40 + (40 * ledPercentage), 127 + (127 * ledPercentage));
      } else {
        color = pixels.Color(80 * ledPercentage, 80 * ledPercentage, 255 * ledPercentage);
      }
      pixels.setPixelColor(i, color);
      pixels.setPixelColor(mirrorId - i, color);
      pixels.setPixelColor(frontId + i, color);
      pixels.setPixelColor(frontId + mirrorId - i, color);
    } else { // Off
      uint32_t color = pixels.Color(0, 0, 0);
      pixels.setPixelColor(i, color);
      pixels.setPixelColor(mirrorId - i, color);
      pixels.setPixelColor(frontId + i, color);
      pixels.setPixelColor(frontId + mirrorId - i, color);
    }
  }
  pixels.show();
}

void showDrivingLights(float balance) {
  if (balance > 0) {
    drivingLightSection(NUMPIXELS / 2, 255, 255, 255, balance);
  } else {
    drivingLightSection(NUMPIXELS / 2, 255, 255, 255, 0);
  }
  if (balance < 0) {
    drivingLightSection(0, 255, 0, 0, -balance);
  } else {
    drivingLightSection(0, 255, 0, 0, 0);
  }

  pixels.show();
}

void drivingLightSection(int startId, int colorR, int colorG, int colorB, float balance) {
  int stripSize = NUMPIXELS / 2;
  int subStripSize = (stripSize - NUMHEADLIGHTPIXELS) / 2;
  double progress = balance * subStripSize;
  
  for (int i = 0; i < NUMHEADLIGHTPIXELS; i++) {
    pixels.setPixelColor(startId + subStripSize + i, colorR, colorG, colorB);
  }
  
  for (int i = 0; i < subStripSize; i++) {
    if (i < progress - 1.0) {
      pixels.setPixelColor(startId + subStripSize - i- 1, colorR, colorG, colorB);
      pixels.setPixelColor(startId + stripSize - subStripSize + i, colorR, colorG, colorB);
    } else if (i < progress) {
      double ledPercentage = progress - i; // From 0 - 1
      
      uint32_t color = pixels.Color(colorR  * ledPercentage, colorG * ledPercentage, colorB * ledPercentage);
      pixels.setPixelColor(startId + subStripSize - i - 1, color);
      pixels.setPixelColor(startId + stripSize - subStripSize + i, color);
    } else {
      pixels.setPixelColor(startId + subStripSize - i- 1, 0, 0, 0);
      pixels.setPixelColor(startId + stripSize - subStripSize + i, 0, 0, 0);
    }
  }
}

void sweepToDrivingMode(double powerOf, int milliSeconds) {
  startTime = millis();

  float percent = 0.0;
  while(percent <= 1.0) {
    double progress = percent * (NUMPIXELS / 2 + 1);
    for (int i = 0; i < NUMPIXELS / 2 - 1; i++) {
      int leftId = getLeftLedId(i);
      int rightId = getRightLedId(i);
      
      if (i <= progress - 1) { // After sweep: Full 'to' color
        if (isHeadLightId(leftId)) {
          pixels.setPixelColor(leftId, pixels.Color(255, 255, 255));
          pixels.setPixelColor(rightId, pixels.Color(255, 255, 255));
        } else if (isTailLightId(leftId)) {
          pixels.setPixelColor(leftId, pixels.Color(255, 0, 0));
          pixels.setPixelColor(rightId, pixels.Color(255, 0, 0));
        } else {
          pixels.setPixelColor(leftId, pixels.Color(0, 0, 0));
          pixels.setPixelColor(rightId, pixels.Color(0, 0, 0));
        }
      } else if (i < progress) { // Transition color
        double ledPercentage = progress - i; // From 0 - 1
        if (isHeadLightId(leftId)) {
          uint32_t color = pixels.Color(255 * ledPercentage, 255, 255 * ledPercentage);
          pixels.setPixelColor(leftId, color);
          pixels.setPixelColor(rightId, color);
        } else if (isTailLightId(leftId)) {
          uint32_t color = pixels.Color(255 * ledPercentage, 255 - (255 * ledPercentage), 0);
          pixels.setPixelColor(leftId, color);
          pixels.setPixelColor(rightId, color);
        } else {
          uint32_t color = pixels.Color(0, 255 - (255 * ledPercentage), 0);
          pixels.setPixelColor(leftId, color);
          pixels.setPixelColor(rightId, color);
        }
      } else { // Before sweep: Full 'from' color
        pixels.setPixelColor(leftId, pixels.Color(0, 255, 0));
        pixels.setPixelColor(rightId, pixels.Color(0, 255, 0));
      }
    }
    pixels.show();
    delay(5);
    
    double linear = (double) (millis() - startTime) / (double) milliSeconds;
    percent = pow(linear, powerOf);
  }
}

void showBalanceLights(float balance) {
  balance = (balance + 1) / 2; // Refactor to float 0..1
  double progress = balance * NUMPIXELS / 2;
  int offset = NUMPIXELS / 4;
  for(int i = 0; i<NUMPIXELS / 2; i++) {
    
    int nId1 = i + offset;
    int nId2 = i + offset + 1;
    int mId1 = offset + offset - nId1;
    int mId2 = offset + offset - nId2;
    if (mId1 < 0) mId1 += NUMPIXELS;
    if (mId2 < 0) mId2 += NUMPIXELS;
    
    if (i < progress - 1.0) { // Before
      pixels.setPixelColor(nId1, pixels.Color(0, 0, 0));
      pixels.setPixelColor(mId1, pixels.Color(0, 0, 0));
    } else if (i <= progress) { // Light
      double next = (progress - i);
      double current = 1.0f - next;
      
      uint32_t color1 = pixels.Color(80 * current, 80 * current, 255 * current);
      uint32_t color2 = pixels.Color(80 * next, 80 * next, 255 * next);
      
      pixels.setPixelColor(nId1, color1);
      pixels.setPixelColor(nId2, color2);

      pixels.setPixelColor(mId1, color1);
      pixels.setPixelColor(mId2, color2);
    } else { // After
      pixels.setPixelColor(nId2, pixels.Color(0, 0, 0));
      pixels.setPixelColor(mId2, pixels.Color(0, 0, 0));
    }
  }
  pixels.show();
}

void lightsOff() {
  showColor(0, 0, 0);
}

void showColor(int colorR, int colorG, int colorB) {
  for (int i = 0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, colorR, colorG, colorB);
  }
  pixels.show();
}

void showAlert(int fromR, int fromG, int fromB, int toR, int toG, int toB, int milliSeconds) {
  int id1 = 0;
  int id2 = NUMPIXELS / 2 - 1;
  int id3 = NUMPIXELS / 2;
  int id4 = NUMPIXELS - 1;
  
  startTime = millis();
  while(millis() < startTime + milliSeconds) {
    float progress = (float) (millis() - startTime) / milliSeconds;
    uint32_t color = pixels.Color((toR - fromR) * progress + fromR, (toG - fromG) * progress + fromG, (toB - fromB) * progress + fromB);
    pixels.setPixelColor(id1, color);
    pixels.setPixelColor(id2, color);
    pixels.setPixelColor(id3, color);
    pixels.setPixelColor(id4, color);
    pixels.show();
    delay(1);
  }
  uint32_t color = pixels.Color(toR, toG, toB);
  pixels.setPixelColor(id1, color);
  pixels.setPixelColor(id2, color);
  pixels.setPixelColor(id3, color);
  pixels.setPixelColor(id4, color);
  pixels.show();
}

// Turn i int id with offset and loop within numpixels
int getLedId(int i) {
  int ledId = i + ID_OFFSET;
  if (ledId < 0) {
    ledId += NUMPIXELS;
  } else if (ledId >= NUMPIXELS) {
    ledId -= NUMPIXELS;
  }
  return ledId;
}

// Get mirror led pixel id, inverse from i
int getLeftLedId(int i) {
  int ledId = ID_OFFSET + i;
  if (ledId < 0) {
    ledId += NUMPIXELS;
  } else if (ledId >= NUMPIXELS) {
    ledId -= NUMPIXELS;
  }
  return ledId;
}
int getRightLedId(int i) {
  int ledId = ID_OFFSET - i;
  if (ledId < 0) {
    ledId += NUMPIXELS;
  } else if (ledId >= NUMPIXELS) {
    ledId -= NUMPIXELS;
  }
  return ledId;
}

boolean isHeadLightId(int ledId) {
  int stripSize = NUMPIXELS / 2;
  int subStripSize = (stripSize - NUMHEADLIGHTPIXELS) / 2;
  if(ledId >= stripSize + subStripSize && ledId < stripSize + subStripSize + NUMHEADLIGHTPIXELS) {
    return true;
  }
  return false;
}

boolean isTailLightId(int ledId) {
  int stripSize = NUMPIXELS / 2;
  int subStripSize = (stripSize - NUMHEADLIGHTPIXELS) / 2;
  if(ledId >= subStripSize && ledId < subStripSize + NUMHEADLIGHTPIXELS) {
    return true;
  }
  return false;
}
