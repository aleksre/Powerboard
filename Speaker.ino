#include "pitches.h"

int speakerPin = 8;

void playActivateSE() {
  tone(speakerPin, NOTE_A3, 1000/12);
  delay(1000/12);
  tone(speakerPin, NOTE_CS4, 1000/12);
  delay(1000/12);
  tone(speakerPin, NOTE_E4, 1000/12);
  delay(1000/12);
  tone(speakerPin, NOTE_A4, 1000/12);
  delay(1000/12);
  noTone(speakerPin);
}

void playDeactivateSE() {
  tone(speakerPin, NOTE_A4, 1000/8);
  delay(1000/8); 
  tone(speakerPin, NOTE_A3, 1000/8);
  delay(1000/8);
  noTone(speakerPin);
}

void playConnectSE() {
  tone(speakerPin, NOTE_A3, 1000/8);
  delay(1000/8);
  tone(speakerPin, NOTE_E4, 1000/8);
  delay(1000/8);
  noTone(speakerPin);
}

void playAlertSE() {
  tone(speakerPin, NOTE_E4, 1000/8);
  delay(1000/8);
  noTone(speakerPin);
}

void playCalibrationSE() {
  tone(speakerPin, NOTE_CS4, 1000/8);
  delay(1000/8);
  tone(speakerPin, NOTE_A4, 1000/8);
  delay(1000/8);
  tone(speakerPin, NOTE_CS5, 1000/8);
  delay(1000/8);
  noTone(speakerPin);
}
