// ===================================================
//  DINO BALL RUSH
//  Collect balls, dodge cacti. 3 lives, then GAME OVER.
//  JUMP button: swap rows / start / restart
// ===================================================
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);   // try 0x3F if the screen stays blank

const int jumpButton  = 2;
const int leftButton  = 3;
const int rightButton = 4;

// Columns 0-4 = score + lives, columns 5-15 = play field
const int FIELD_START = 5;
const int FIELD_END   = 15;

const int MAX_LIVES = 3;
const int MAX_OBS   = 4;
const unsigned long OBS_SPEED    = 350;   // lower = faster cacti
const unsigned long COIN_TIMEOUT = 4000;  // how long a ball stays (ms)

// Custom characters
byte dinoChar[8]   = { B00111, B00101, B00111, B10110, B11100, B01100, B01010, B10011 };
byte ballChar[8]   = { B00000, B00000, B01110, B11111, B11111, B01110, B00000, B00000 };
byte cactusChar[8] = { B00100, B00101, B10101, B10101, B10111, B01100, B00100, B00100 };
byte heartChar[8]  = { B00000, B01010, B11111, B11111, B11111, B01110, B00100, B00000 };

enum State { TITLE, PLAYING, GAME_OVER };
State state = TITLE;

int dinoCol, dinoRow;
int coinCol, coinRow;
bool coinActive;
int score, lives;

int obsCol[MAX_OBS];
int obsRow[MAX_OBS];
bool obsActive[MAX_OBS];

unsigned long lastObsMove, nextObsSpawn, nextCoinTime, coinSpawnTime, lastMove;
bool prevJump = HIGH;

// ===================================================
void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(jumpButton, INPUT_PULLUP);
  pinMode(leftButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);

  lcd.createChar(0, dinoChar);
  lcd.createChar(1, ballChar);
  lcd.createChar(2, cactusChar);
  lcd.createChar(3, heartChar);

  randomSeed(analogRead(A0));
  showTitle();
}

// ===================================================
void loop() {
  if (state == PLAYING) {
    handleMoves();
    updateGame();
    if (state == PLAYING) drawGame();   // don't draw over the GAME OVER screen
  }
  else if (jumpPressed()) {             // TITLE or GAME_OVER: wait for JUMP
    startGame();
  }
}

// ===================================================
bool jumpPressed() {
  bool now = digitalRead(jumpButton);
  bool pressed = (prevJump == HIGH && now == LOW);
  prevJump = now;
  if (pressed) delay(30);               // debounce
  return pressed;
}

// ===================================================
void showTitle() {
  state = TITLE;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(1));
  lcd.print(" DINO BALL RUSH");
  lcd.setCursor(0, 1);
  lcd.print("Press JUMP");
}

void showGameOver() {
  state = GAME_OVER;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME OVER!");
  lcd.setCursor(0, 1);
  lcd.print("Score:");
  lcd.print(score);
  lcd.setCursor(10, 1);
  lcd.print("JUMP>>");
}

void startGame() {
  randomSeed(micros());
  state = PLAYING;
  score = 0;
  lives = MAX_LIVES;
  dinoCol = 9;
  dinoRow = 1;
  coinActive = false;
  for (int i = 0; i < MAX_OBS; i++) obsActive[i] = false;

  unsigned long now = millis();
  lastObsMove  = now;
  nextObsSpawn = now + 1500;
  nextCoinTime = now + 500;
  lcd.clear();
}

// ===================================================
void handleMoves() {
  if (jumpPressed()) dinoRow = 1 - dinoRow;

  if (millis() - lastMove >= 110) {
    if (digitalRead(leftButton) == LOW && dinoCol > FIELD_START) {
      dinoCol--;
      lastMove = millis();
    } else if (digitalRead(rightButton) == LOW && dinoCol < FIELD_END) {
      dinoCol++;
      lastMove = millis();
    }
  }
}

void updateGame() {
  unsigned long now = millis();

  // Move cacti left
  if (now - lastObsMove >= OBS_SPEED) {
    lastObsMove = now;
    for (int i = 0; i < MAX_OBS; i++) {
      if (obsActive[i]) {
        obsCol[i]--;
        if (obsCol[i] < FIELD_START) obsActive[i] = false;
      }
    }
  }

  // Spawn a cactus
  if (now >= nextObsSpawn) {
    for (int i = 0; i < MAX_OBS; i++) {
      if (!obsActive[i]) {
        obsActive[i] = true;
        obsCol[i] = FIELD_END;
        obsRow[i] = random(0, 2);
        break;
      }
    }
    nextObsSpawn = now + OBS_SPEED * random(3, 7);
  }

  // Spawn / expire the ball
  if (!coinActive && now >= nextCoinTime) {
    coinCol = random(FIELD_START, FIELD_END + 1);
    coinRow = random(0, 2);
    if (!(coinCol == dinoCol && coinRow == dinoRow)) {
      coinActive = true;
      coinSpawnTime = now;
    }
  } else if (coinActive && now - coinSpawnTime > COIN_TIMEOUT) {
    coinActive = false;
    nextCoinTime = now + random(500, 1500);
  }

  // Collect ball
  if (coinActive && dinoCol == coinCol && dinoRow == coinRow) {
    score++;
    coinActive = false;
    nextCoinTime = now + random(400, 1200);
  }

  // Hit a cactus
  for (int i = 0; i < MAX_OBS; i++) {
    if (obsActive[i] && obsCol[i] == dinoCol && obsRow[i] == dinoRow) {
      obsActive[i] = false;
      lives--;
      if (lives <= 0) {
        showGameOver();
        return;
      }
    }
  }
}

// ===================================================
void drawGame() {
  // Score and lives
  lcd.setCursor(0, 0);
  lcd.print("S:");
  lcd.print(score);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  for (int i = 0; i < MAX_LIVES; i++) {
    if (i < lives) lcd.write(byte(3));
    else lcd.print(" ");
  }

  // Play field
  for (int r = 0; r < 2; r++) {
    lcd.setCursor(FIELD_START, r);
    for (int c = FIELD_START; c <= FIELD_END; c++) {
      byte ch = ' ';
      for (int i = 0; i < MAX_OBS; i++) {
        if (obsActive[i] && obsCol[i] == c && obsRow[i] == r) ch = 2;
      }
      if (coinActive && coinCol == c && coinRow == r) ch = 1;
      if (dinoCol == c && dinoRow == r) ch = 0;
      lcd.write(ch);
    }
  }
}
