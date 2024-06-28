#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 3  // Number of matrices

// Pins for LED matrix
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// Create a new instance of the MD_Parola class with hardware SPI connection
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup() {
  // Initialize the display
  myDisplay.begin();
  myDisplay.setIntensity(0); // Set brightness level (0-15)
  myDisplay.displayClear();

  // Set text alignment and speed
  myDisplay.setTextAlignment(PA_CENTER);
  myDisplay.setSpeed(10); // Adjust speed for scrolling effect (lower value is faster)
}

void loop() {
  // Display first message
  if (myDisplay.displayAnimate()) {
    myDisplay.displayText("EDUSTEP.UZ", PA_LEFT, myDisplay.getSpeed(), PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }

  // Add delay to switch between messages
  delay(250);

  // Display second message
  if (myDisplay.displayAnimate()) {
    myDisplay.displayText("EDUSTEP", PA_LEFT, myDisplay.getSpeed(), PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }

  // Add delay before repeating the loop
  delay(250);
}