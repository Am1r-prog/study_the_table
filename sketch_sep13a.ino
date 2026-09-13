#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Change to 0x3F if 0x27 shows blank
Servo leftServo;
Servo rightServo;

const int IR_BOOK = 2; // Bottom sensor (~1 cm)
const int IR_HAND = 3; // Top sensor (~4-5 cm)

const int SERVO_LEFT_PIN  = 9;
const int SERVO_RIGHT_PIN = 10;

// Resting neutral angles (level table)
const int REST_LEFT  = 90;
const int REST_RIGHT = 90;

// Angle offset (+/- 35 degrees)
const int TILT_OFFSET = 35;

const int BLOCKED = LOW;

int currentMode = -1;

void setup() {
  pinMode(IR_BOOK, INPUT);
  pinMode(IR_HAND, INPUT);

  leftServo.attach(SERVO_LEFT_PIN);
  rightServo.attach(SERVO_RIGHT_PIN);

  // Set table level at start
  leftServo.write(REST_LEFT);
  rightServo.write(REST_RIGHT);

  lcd.init();
  lcd.backlight();
  setMode(0);
}

void setMode(int mode) {
  if (currentMode == mode) return;
  currentMode = mode;
  lcd.clear();

  switch (mode) {
    case 0: // Empty table
      lcd.setCursor(0, 0);
      lcd.print("VAA MOWNE");
      lcd.setCursor(0, 1);
      lcd.print("PADIKAMM");
      break;

    case 1: // Book on desk, safe
      lcd.setCursor(0, 0);
      lcd.print("YEZHUTHI PADIK");
      lcd.setCursor(0, 1);
      lcd.print("MOWNEEE...");
      break;

    case 2: // Book + hand detected together - Attack
      lcd.setCursor(0, 0);
      lcd.print("ENTHA MOWNEE");
      lcd.setCursor(0, 1);
      lcd.print("PADIKUNILLE");
      break;
  }
}

// Moves both servo arms in the same physical vertical direction
void rockTable() {
  // Both sides move UP together
  leftServo.write(REST_LEFT + TILT_OFFSET);
  rightServo.write(REST_RIGHT - TILT_OFFSET);
  delay(120);

  // Both sides move DOWN together
  leftServo.write(REST_LEFT - TILT_OFFSET);
  rightServo.write(REST_RIGHT + TILT_OFFSET);
  delay(120);
}

void loop() {
  bool bookPlaced     = (digitalRead(IR_BOOK) == BLOCKED);
  bool handApproached = (digitalRead(IR_HAND) == BLOCKED);

  // 1. Book is placed AND hand is hovering over it -> rock continuously
  if (bookPlaced && handApproached) {
    setMode(2);
    rockTable();
  }
  // 2. Only book is on the table (no hand) -> level and still
  else if (bookPlaced && !handApproached) {
    setMode(1);
    leftServo.write(REST_LEFT);
    rightServo.write(REST_RIGHT);
    delay(40);
  }
  // 3. No book on desk -> level and calm
  else {
    setMode(0);
    leftServo.write(REST_LEFT);
    rightServo.write(REST_RIGHT);
    delay(40);
  }
}