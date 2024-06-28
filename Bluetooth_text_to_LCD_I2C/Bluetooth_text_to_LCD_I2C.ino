#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust the address if necessary

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("Bluetooth...");

  while (!Serial) {
    ;  // Wait for serial port to connect
  }
}

void loop() {
  if (Serial.available()) {
    lcd.clear();  // Clear the previous display

    String incomingText = "";
    while (Serial.available()) {
      char c = (char)Serial.read();
      incomingText += c;
      delay(5);  // Delay for proper reception of characters
    }

    lcd.setCursor(0, 0);  // Set the cursor to the first row
    if (incomingText.length() <= 16) {
      lcd.print(incomingText);  // Print the text if it fits in one row
    } else {
      lcd.print(incomingText.substring(0, 16));  // Print the first 16 characters on the first row
      lcd.setCursor(0, 1);  // Move cursor to the second row
      lcd.print(incomingText.substring(16));  // Print the rest of the text
    }
  }
}