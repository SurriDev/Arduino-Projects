#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
#define GREEN_LED 6
#define RED_LED 7
#define BUZZER 5

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial BTSerial(2, 3); // RX, TX

int PIN = "";
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 8, A4, A5}; // Connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String correctPIN = "10643360"; // Example PIN for Bluetooth unlock
String correctNumpadCode = "124578"; // Example code for numpad unlock
byte allowedUID[4] = {0x03, 0x70, 0xEB, 0xFD}; // Example UID for RFID card

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.begin(9600);
  BTSerial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Scan card or en-");
  lcd.setCursor(0, 1);
  lcd.print("ter PIN-code:");
}

void loop() {
  // Bluetooth Unlocking
  if (BTSerial.available()) {
    String receivedPIN = BTSerial.readString();
    lcd.print("Received Bluetooth PIN: ");
    lcd.println(receivedPIN); // Debugging
    if (receivedPIN.equals(correctPIN)) {
      unlockDoor("Bluetooth");
    } else {
      accessDenied();
    }
  }

  // RFID Unlocking
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (checkRFIDCard(rfid.uid.uidByte)) {
      unlockDoor("RFID Card");
    } else {
      accessDenied();
    }
    rfid.PICC_HaltA();
  }

  // Numpad Unlocking
  char key = keypad.getKey();
  if (key) {
    static String numpadInput = "";
    lcd.clear();
    lcd.print("PIN-code: ");
    lcd.setCursor(0, 1);
    lcd.print(numpadInput + key); // Debugging
    if (key == '*') {
      if (numpadInput.equals(correctNumpadCode)) {
        unlockDoor("Numpad");
      } else {
        accessDenied();
      }
      numpadInput = "";
    } else {
      numpadInput += key;
    }
  }
}

void unlockDoor(String method) {
  digitalWrite(GREEN_LED, HIGH);
  tone(BUZZER, 1000, 200);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Granted!");
  lcd.setCursor(0, 1);
  lcd.print("Method:" + method);
  delay(3000);
  digitalWrite(GREEN_LED, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan card or en-");
  lcd.setCursor(0, 1);
  lcd.print("ter PIN-code:");}

void accessDenied() {
  digitalWrite(RED_LED, HIGH);
  tone(BUZZER, 500, 200);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Denied!");
  delay(3000);
  digitalWrite(RED_LED, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan card or en-");
  lcd.setCursor(0, 1);
  lcd.print("ter PIN-code:");}

bool checkRFIDCard(byte *uid) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != allowedUID[i]) {
      return false;
    }
  }
  return true;
}