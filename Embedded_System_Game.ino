//By Andy Lehmann
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define seg_c B00000100
#define seg_b B00000010
#define seg_g digitalWrite(2,HIGH)
#define seg_dp digitalWrite(3,HIGH)
#define seg_a B00000001
#define seg_f B00100000
#define seg_e B00010000
#define seg_d B00001000
#define VRX_PIN A0
#define VRY_PIN A1
#define joystick_button 4
#define tranducer 7
#define SNAKE_MAX_LENGTH 10
#define vertical_neutral 498
#define horizontal_neutral 512
#define offset 5

LiquidCrystal_I2C lcd(0x27, 16, 2);

int snakeSpeed = 200; // Adjust this to change the snake speed
int snakeLength = 1;
int snakeMax = 128;
int snake[128][2]; // Snake positions
int food[128][2];       // Food position

int direction = 0;
int lastDirection = 1;     
int foodX, foodY;       // Food position
int score = 0;
int xValue, yValue;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Snake Game");
  lcd.setCursor(0, 1);
  lcd.print("Press to start");
  pinMode(joystick_button, INPUT_PULLUP);
  pinMode(tranducer, OUTPUT);
}

void readJoystick() {
  int xValue = analogRead(VRX_PIN);
  int yValue = analogRead(VRY_PIN);

  if (xValue < 100 && lastDirection != 1) {
    direction = 3; // Left
  } else if (xValue > 900 && lastDirection != 3) {
    direction = 1; // Right
  } else if (yValue < 100 && lastDirection != 2) {
    direction = 0; // Up
  } else if (yValue > 900 && lastDirection != 0) {
    direction = 2; // Down
  }
}

void initializeGame() {
  snakeLength = 1;
  snake[0][0] = 5;
  snake[0][1] = 0;
  food[0][0] = 0;
  food[0][1] = 8;
  direction = 1;
  lastDirection = 1;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Snake Game");
  lcd.setCursor(0, 1);
  lcd.print("Press to start");
}

void moveSnake() {
 lastDirection = direction;

  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i][0] = snake[i - 1][0];
    snake[i][1] = snake[i - 1][1];
  }

  switch (direction) {
    case 0: // Up
      snake[0][1]++;
      break;
    case 1: // Right
      snake[0][0]++;
      break;
    case 2: // Down
      snake[0][1]--;
      break;
    case 3: // Left
      snake[0][0]--;
      break;
  }
}

void checkCollision() {
  // Check collision with walls
  if (snake[0][0] < 0 || snake[0][0] >= 16 || snake[0][1] < 0 || snake[0][1] >= 2) {
    gameover();
  }

  // Check collision with itself
  for (int i = 1; i < snakeLength; i++) {
    if (snake[0][0] == snake[i][0] && snake[0][1] == snake[i][1]) {
      gameover();
    }
  }
}

void checkFoodCollision() {
  // Check collision with food
  if (snake[0][0] == food[0] && snake[0][1] == food[1]) {
    snakeLength++;
    playEatSound();
    generateFood();
  }
}

void generateFood() {
  // Print the food on the LCD
  lcd.setCursor(food[0][0] = random(0, 16), food[0][1] = random(0, 2));
  lcd.print("*");
}

void displayGame() {
  lcd.clear();

  for (int i = 0; i < snakeLength; i++) {
    lcd.setCursor(snake[i][0], snake[i][1]);
    lcd.print("#");
  }
}

void updateScoreDisplay() {
  displayDigit(score);
}

void playEatSound() {
  tone(tranducer, 1000, 200);
  delay(200);
  noTone(tranducer);
}

void displayDigit(int digit)
{
  switch(digit)
    {
      case 0:
        PORTB = (seg_a | seg_b | seg_c | seg_d | seg_e | seg_f);
        break;
      case 1:
        PORTB = (seg_b | seg_c);
        break;
      case 2:
        PORTB = (seg_a | seg_b | seg_e | seg_d), seg_g;
        break;
      case 3:
        PORTB = (seg_a | seg_b | seg_c | seg_d), seg_g;
        break;
      case 4:
        PORTB = (seg_f | seg_b | seg_c), seg_g;
        break;
      case 5:
        PORTB = (seg_a | seg_f | seg_c | seg_d), seg_g;
        break;
      case 6:
        PORTB = (seg_a | seg_f | seg_e | seg_d | seg_c), seg_g;
        break;
      case 7:
        digitalWrite(2, LOW);
        PORTB = (seg_a | seg_b | seg_c);
        break;
      case 8:
        PORTB = (seg_a | seg_f | seg_e | seg_d | seg_c | seg_b), seg_g;
        break;
      case 9:
        PORTB = (seg_a | seg_f | seg_b | seg_c | seg_d), seg_g;
        break;
    }
}


void gameover() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  delay(2000);
}

void loop() {
  if (digitalRead(joystick_button) == LOW) {
    lcd.clear();
    delay(1000);

    initializeGame();
  }

  readJoystick();
  moveSnake();
  checkCollision();
  generateFood();
  displayGame();
  delay(snakeSpeed);
}