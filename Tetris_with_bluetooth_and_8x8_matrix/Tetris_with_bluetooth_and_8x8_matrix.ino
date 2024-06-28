#include <LedControl.h>  // Library for MAX7219 LED matrix
#include <SoftwareSerial.h>  // Library for Bluetooth communication

// Define pins for LED matrix
const int DIN_PIN = 2;  // Data In
const int CLK_PIN = 3;  // Clock
const int CS_PIN = 4;   // Chip Select

// Define Bluetooth pins
const int BT_RX_PIN = 6;  // RX pin of Bluetooth module
const int BT_TX_PIN = 7;  // TX pin of Bluetooth module

// Define buzzer pin
const int BUZZER_PIN = 8;

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);  // Create a LedControl instance
SoftwareSerial bluetooth(BT_RX_PIN, BT_TX_PIN);  // Create a SoftwareSerial instance for Bluetooth communication

// Tetris game variables
byte gameBoard[8][8];  // Game board represented as an 8x8 matrix
int currentPiece[4][4];  // Current falling piece
int currentX = 3, currentY = 0;  // Initial position of the falling piece
long lastMoveTime = 0;  // Time tracking for piece movement
int score = 0;  // Game score
bool gameOver = false;  // Game over flag

// Tetromino shapes
const int tetrominoes[9][4][4] = {
  {{1, 1, 1, 1},  // I
   {0, 0, 0, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0}},
  {{1, 1, 1, 0},  // J
   {0, 0, 1, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0}},
  {{1, 1, 1, 0},  // L
   {1, 0, 0, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0}},
  {{1, 1, 0, 0},  // O
   {1, 1, 0, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0}},
  {{0, 1, 1, 0},  // S
   {1, 1, 0, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0}},
  {{1, 1, 0, 0},  // T
   {0, 1, 1, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0}},
  {{1, 1, 0, 0},  // Z
   {0, 1, 1, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0}},
  {{1, 0, 0, 0},  // New block 1
   {1, 0, 0, 0},
   {1, 0, 0, 0},
   {1, 0, 0, 0}},
  {{1, 0, 0, 0},  // New block 2
   {1, 0, 0, 0},
   {1, 0, 0, 0},
   {0, 0, 0, 0}}
};

const int GAME_SPEED = 500;  // Initial game speed (milliseconds)

char command;  // Command received from Bluetooth

void setup() {
  // Initialize LED matrix
  lc.shutdown(0, false);  // Wake up MAX7219
  lc.setIntensity(0, 8);  // Set brightness level (0 is min, 15 is max)
  lc.clearDisplay(0);  // Clear display

  // Initialize Bluetooth communication
  bluetooth.begin(9600);
  
  // Initialize buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize game board
  clearGameBoard();
  
  // Seed random number generator
  randomSeed(analogRead(0));
  
  // Start the game
  spawnNewPiece();
}

void loop() {
  // Check for Bluetooth commands
  if (bluetooth.available() > 0) {
    command = bluetooth.read();
    processCommand(command);
  }
  
  // Handle game timing if game is not over
  if (!gameOver && millis() - lastMoveTime > GAME_SPEED) {
    lastMoveTime = millis();
    
    // Move piece down
    if (checkCollision(currentX, currentY + 1, currentPiece)) {
      mergePiece();
      clearLines();
      if (!spawnNewPiece()) {
        gameOver = true;
        playGameOverSound();
      }
    } else {
      currentY++;
      playMoveSound();
    }
    
    // Update display
    updateDisplay();
  }
}

bool spawnNewPiece() {
  int shapeIndex = random(9);  // Adjusted for the number of shapes (0 to 8)
  memcpy(currentPiece, tetrominoes[shapeIndex], sizeof(currentPiece));
  currentX = 3;
  currentY = 0;
  
  // Check if new piece can spawn, return false if game over
  if (checkCollision(currentX, currentY, currentPiece)) {
    return false;  // Game over
  }
  
  return true;  // Piece spawned successfully
}

void mergePiece() {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (currentPiece[row][col] != 0) {
        // Check boundaries before setting on game board
        if (currentY + row >= 0 && currentY + row < 8 && currentX + col >= 0 && currentX + col < 8) {
          gameBoard[currentY + row][currentX + col] = 1;
        }
      }
    }
  }
}

void clearLines() {
  for (int row = 7; row >= 0; row--) {
    bool lineIsFull = true;
    for (int col = 0; col < 8; col++) {
      if (gameBoard[row][col] == 0) {
        lineIsFull = false;
        break;
      }
    }
    if (lineIsFull) {
      // Shift down all rows above this one
      for (int r = row; r > 0; r--) {
        for (int c = 0; c < 8; c++) {
          gameBoard[r][c] = gameBoard[r - 1][c];
        }
      }
      // Clear top row
      for (int c = 0; c < 8; c++) {
        gameBoard[0][c] = 0;
      }
      score += 10;  // Increase score for each cleared line
      playClearLineSound();
      row++;  // Check the same row again after shifting down
    }
  }
  
  // Check for game over if pieces reach the top
  if (currentY == 0 && checkCollision(currentX, currentY, currentPiece)) {
    gameOver = true;
    playGameOverSound();
  }
}

void updateDisplay() {
  // Clear display first
  lc.clearDisplay(0);

  // Display game board on LED matrix
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, gameBoard[row][col]);
    }
  }

  // Display current piece on LED matrix
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (currentPiece[row][col] != 0) {
        // Calculate LED matrix coordinates
        int matrixX = currentX + col;
        int matrixY = currentY + row;
        
        // Check boundaries before setting on LED matrix
        if (matrixY >= 0 && matrixY < 8 && matrixX >= 0 && matrixX < 8) {
          lc.setLed(0, matrixY, matrixX, 1);
        }
      }
    }
  }
  
  // Display game over message if game is over
  if (gameOver) {
    lc.setRow(0, 0, B11111111);  // Display "GAME"
    lc.setRow(0, 1, B01011010);  // Display "OVER"
  }
}

void processCommand(char command) {
  if (!gameOver) {
    switch (command) {
      case 'L':  // Move left
        if (!checkCollision(currentX - 1, currentY, currentPiece)) {
          currentX--;
          playMoveSound();
        }
        break;
      case 'R':  // Move right
        if (!checkCollision(currentX + 1, currentY, currentPiece)) {
          currentX++;
          playMoveSound();
        }
        break;
      case 'D':  // Move down faster
        if (!checkCollision(currentX, currentY + 1, currentPiece)) {
          currentY++;
          playMoveSound();
        }
        break;
      case 'X':  // Rotate clockwise
        rotatePieceClockwise();
        if (checkCollision(currentX, currentY, currentPiece)) {
          rotatePieceCounterClockwise();
        } else {
          playRotateSound();
        }
        break;
      case 'Z':  // Rotate counterclockwise
        rotatePieceCounterClockwise();
        if (checkCollision(currentX, currentY, currentPiece)) {
          rotatePieceClockwise();
        } else {
          playRotateSound();
        }
        break;
    }
  }
}

bool checkCollision(int newX, int newY, int piece[4][4]) {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (piece[row][col] != 0) {
        // Check boundaries and collision with other pieces
        if (newX + col < 0 || newX + col >= 8 || newY + row >= 8 || (newY + row >= 0 && gameBoard[newY + row][newX + col] != 0)) {
          return true;
        }
      }
    }
  }
  return false;
}

void rotatePieceClockwise() {
  int tempPiece[4][4];
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      tempPiece[col][3 - row] = currentPiece[row][col];
    }
  }
  memcpy(currentPiece, tempPiece, sizeof(currentPiece));
}

void rotatePieceCounterClockwise() {
  int tempPiece[4][4];
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      tempPiece[3 - col][row] = currentPiece[row][col];
    }
  }
  memcpy(currentPiece, tempPiece, sizeof(currentPiece));
}

void clearGameBoard() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      gameBoard[row][col] = 0;
    }
  }
}

void playMoveSound() {
  tone(BUZZER_PIN, 400, 50);  // Play tone at 400 Hz for 50 ms
}

void playRotateSound() {
  tone(BUZZER_PIN, 600, 50);  // Play tone at 600 Hz for 50 ms
}

void playClearLineSound() {
  tone(BUZZER_PIN, 800, 100);  // Play tone at 800 Hz for 100 ms
}

void playGameOverSound() {
  tone(BUZZER_PIN, 200, 500);  // Play lower tone at 200 Hz for 500 ms for game over
}