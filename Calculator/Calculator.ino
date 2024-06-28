#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Initialize the LCD with the I2C address 0x27 and size 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad setup
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'C','0','=','/'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; // Rows connected to Arduino pins
byte colPins[COLS] = {6, 7, 8, 9}; // Columns connected to Arduino pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String inputString = "";
char lastOperator = '\0';
float firstNumber = 0;
float secondNumber = 0;
float result = 0;

void setup() {
  lcd.init();  // Specify the number of columns and rows
  lcd.backlight();
  lcd.print("Calculator");
  delay(2000);
  lcd.clear();
  lcd.print("Enter Numbers:");
  lcd.setCursor(0, 1);
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    if (key >= '0' && key <= '9') {
      inputString += key;
      lcd.setCursor(0, 1);
      lcd.print(inputString);
    } else if (key == 'C') {
      inputString = "";
      firstNumber = 0;
      secondNumber = 0;
      result = 0;
      lastOperator = '\0';
      lcd.clear();
      lcd.print("Enter Numbers:");
      lcd.setCursor(0, 1);
    } else if (key == '=') {
      if (lastOperator != '\0') {
        secondNumber = inputString.toFloat();
        inputString = "";

        switch (lastOperator) {
          case '+':
            result = firstNumber + secondNumber;
            break;
          case '-':
            result = firstNumber - secondNumber;
            break;
          case '*':
            result = firstNumber * secondNumber;
            break;
          case '/':
            if (secondNumber != 0) {
              result = firstNumber / secondNumber;
            } else {
              lcd.clear();
              lcd.print("Error: Div by 0");
              delay(2000);
              lcd.clear();
              lcd.print("Enter Numbers:");
              lcd.setCursor(0, 1);
              return;
            }
            break;
        }

        lcd.clear();
        lcd.print("Result:");
        lcd.setCursor(0, 1);

        char resultStr[16];
        dtostrf(result, 0, 2, resultStr); // Convert float to string with 2 decimal places
        lcd.print(resultStr);
        
        delay(3000);
        lcd.clear();
        lcd.print("Enter Numbers:");
        lcd.setCursor(0, 1);
        firstNumber = 0;
        secondNumber = 0;
        result = 0;
        lastOperator = '\0';
      }
    } else {
      if (inputString.length() > 0) {
        firstNumber = inputString.toFloat();
        inputString = "";
        lastOperator = key;
        lcd.setCursor(0, 1);
        lcd.print(firstNumber);
        lcd.print(lastOperator);
      }
    }
  }
}