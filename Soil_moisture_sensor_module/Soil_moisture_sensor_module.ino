#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int sensorPin = A0; // Analog pin connected to AO
const int digitalPin = 7; // Digital pin connected to DO
const int redLEDPin = 8;  // Digital pin connected to red LED
const int greenLEDPin = 9; // Digital pin connected to green LED

int sensorValue = 0;      // Variable to store the analog sensor value
int digitalValue = 0;     // Variable to store the digital sensor value

LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  pinMode(digitalPin, INPUT); // Set the digital pin as input
  pinMode(redLEDPin, OUTPUT); // Set the red LED pin as output
  pinMode(greenLEDPin, OUTPUT); // Set the green LED pin as output
  
  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  sensorValue = analogRead(sensorPin); // Read the analog value from the sensor
  digitalValue = digitalRead(digitalPin); // Read the digital value from the sensor
  
  // Print to the Serial Monitor
  Serial.print("Soil Moisture (Analog): ");
  Serial.println(sensorValue); 
  Serial.print("Soil Moisture (Digital): ");
  Serial.println(digitalValue); 
  
  // Display the sensor values on the LCD
  lcd.setCursor(0, 0); // Set the cursor to column 0, line 0
  lcd.print("Moisture: ");
  lcd.setCursor(10, 0); // Set the cursor to column 10, line 0
  lcd.print(sensorValue);
  lcd.setCursor(0, 1); // Set the cursor to column 0, line 1
  lcd.print("Digital: ");
  lcd.setCursor(10, 1); // Set the cursor to column 10, line 1
  lcd.print(digitalValue);

  // Control LEDs based on the digital value
  if (digitalValue == HIGH) {
    digitalWrite(greenLEDPin, HIGH); // Turn on green LED
    digitalWrite(redLEDPin, LOW); // Turn off red LED
  } else {
    digitalWrite(greenLEDPin, LOW); // Turn off green LED
    digitalWrite(redLEDPin, HIGH); // Turn on red LED
  }

  delay(1000); // Wait for a second before taking another reading
}