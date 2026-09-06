#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <Servo.h>
#include <Stepper.h>

const int STEPS_PER_REV = 2048;
const int PAN_LIMIT     = 512;
const int SERVO_PIN     = 6;
const int BTN_LEFT      = 2;
const int BTN_RIGHT     = 3;
const int BTN_HOME      = 4;
const int POT_PIN       = A0;

Stepper pan(STEPS_PER_REV, 8, 10, 9, 11);

Servo tilt;
hd44780_I2Cexp lcd;

long panPos = 0;
int  tiltAngle = 90;
int  lastTilt = -1;
long lastPanShown = -9999;

void setup() {
  lcd.begin(16, 2);
  lcd.clear();

  pinMode(BTN_LEFT,  INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_HOME,  INPUT_PULLUP);

  pan.setSpeed(10);
  tilt.attach(SERVO_PIN);
  tilt.write(tiltAngle);

  lcd.setCursor(0, 0);
  lcd.print("Pan/Tilt ready");
  delay(800);
  lcd.clear();
}

void loop() {
  int raw = analogRead(POT_PIN);
  tiltAngle = map(raw, 0, 1023, 0, 180);

  if (abs(tiltAngle - lastTilt) >= 2) {
    tilt.write(tiltAngle);
    lastTilt = tiltAngle;
  }

  if (digitalRead(BTN_LEFT) == LOW && panPos > -PAN_LIMIT) {
    pan.step(-1);
    panPos--;
  }
  else if (digitalRead(BTN_RIGHT) == LOW && panPos < PAN_LIMIT) {
    pan.step(1);
    panPos++;
  }
  else if (digitalRead(BTN_HOME) == LOW && panPos != 0) {
    int dir = (panPos > 0) ? -1 : 1;
    pan.step(dir);
    panPos += dir;
  }

  updateDisplay();
}

void updateDisplay() {
  long panDeg = (panPos * 360L) / STEPS_PER_REV;

  if (panDeg == lastPanShown && tiltAngle == lastTilt) return;

  lcd.setCursor(0, 0);
  lcd.print("Pan:  ");
  lcd.print(panDeg);
  lcd.print((char)223);
  lcd.print("    ");

  lcd.setCursor(0, 1);
  lcd.print("Tilt: ");
  lcd.print(tiltAngle);
  lcd.print((char)223);
  lcd.print("    ");

  lastPanShown = panDeg;
}
