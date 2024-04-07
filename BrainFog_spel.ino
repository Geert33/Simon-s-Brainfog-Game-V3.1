// Simons Brain Fog Game 3.1 door Geert Pauwels... //
// WERKT ENKEL OP EEN ARDUINO UNO! //

#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

const int speaker = 11;

int LENGTH = 500;
int notes[4] = {100, 350, 600, 850};
int gamepattern[20];
int difficulty = 1;
int currentRound = 0; // Declareer currentRound als een globale variabele

void setup() {
  initializePins();
  Serial.begin(9600);
  initializeLCD();
}

void loop() {
  showMainMenu();
}

void initializePins() {
  for (int i = 2; i < 10; i++) {
    pinMode(i, OUTPUT);
  }
  for (int i = 6; i < 10; i++) {
    pinMode(i, INPUT);
  }
}

void initializeLCD() {
  lcd.begin(16, 2);
  lcd.setBacklight(VIOLET);
  lcd.setCursor(0, 0);
  lcd.print("Druk een toets...");
}

void showMainMenu() {
  while (true) {
    if (digitalRead(6) == HIGH) {
      digitalWrite(2, HIGH); // Zet LED van knop 1 aan
      buttonFeedback(1);
      difficulty = 1;
      startGame();
    }
    if (digitalRead(7) == HIGH) {
      digitalWrite(3, HIGH); // Zet LED van knop 2 aan
      buttonFeedback(2);
      difficulty = 2;
      startGame();
    }
    if (digitalRead(8) == HIGH) {
      digitalWrite(4, HIGH); // Zet LED van knop 3 aan
      buttonFeedback(3);
      difficulty = 3;
      startGame();
    }
    if (digitalRead(9) == HIGH) {
      digitalWrite(5, HIGH); // Zet LED van knop 4 aan
      buttonFeedback(4);
      difficulty = 4;
      startGame();
    }
  }
}

void buttonFeedback(int difficulty) {
  // Speel de toon af
  tone(speaker, 500, 100);
  delay (500);
digitalWrite(2, LOW); // Zet LED van knop 1 uit
digitalWrite(3, LOW); // Zet LED van knop 2 uit
digitalWrite(4, LOW); // Zet LED van knop 3 uit
digitalWrite(5, LOW); // Zet LED van knop 4 uit
  // Laat de geselecteerde moeilijkheidsgraad zien op het display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moeilijkheidsgr.:");
  lcd.setCursor(7, 1);
  lcd.print(difficulty);
  delay(2000); // Laat de moeilijkheidsgraad kort op het scherm zien voor visuele feedback
}

void startGame() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Veel succes...");
  delay(500);
  generateGame();
  playGame();
}

void generateGame() {
  randomSeed(analogRead(1));
  for (int i = 0; i < (difficulty * 3); i++) {
    gamepattern[i] = random(1, 5);
  }
}

void playGame() {
  int roundCount = 1;
  int playerTurn = 0;
  bool buttonPress = false;
  int currentNote;
  int userInput = 0;
  bool loss = false;

  playNotes(1, 180);
  playNotes(2, 180);
  playNotes(3, 180);
  playNotes(4, 180);
  playNotes(1, 180);
  playNotes(2, 180);
  playNotes(3, 180);
  playNotes(4, 180);

  delay(500);

  for (currentRound = 0; currentRound < (difficulty * 3); currentRound++) {
    roundCount += 0;
    playerTurn = 0;
    buttonPress = false;
    userInput = 0;

    delay(1000);

    for (int j = 0; j <= currentRound; j++) {
      playNotes(gamepattern[j], LENGTH);
    }

    while (playerTurn <= currentRound) {
      currentNote = gamepattern[playerTurn];

      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Score:");
      lcd.setCursor(7, 1);
      lcd.print(currentRound * 1000); // Score tonen voor de huidige ronde

      buttonPress = false;

      while (!buttonPress) {
        if (digitalRead(6) == HIGH) {
          buttonPress = true;
          userInput = 1;
        }
        if (digitalRead(7) == HIGH) {
          buttonPress = true;
          userInput = 2;
        }
        if (digitalRead(8) == HIGH) {
          buttonPress = true;
          userInput = 3;
        }
        if (digitalRead(9) == HIGH) {
          buttonPress = true;
          userInput = 4;
        }
      }

      playNotes(userInput, LENGTH);
      if (currentNote == userInput) {
        playerTurn++;
      } else {
        gameOver(false);
        loss = true;
        break;
      }
    }

    if (loss) {
      break;
    }
  }

  if (!loss) {
    lcd.setCursor(0, 0);
    lcd.print("Proficiat!!!");
    gameOver(true);
  }
}

void playNotes(int index, int notespeed) {
  unsigned long noteStartTime = millis();
  tone(speaker, notes[index - 1], notespeed);

  while (millis() - noteStartTime < notespeed) {
    digitalWrite(index + 1, HIGH);
    delay(10);
    digitalWrite(index + 1, LOW);
    delay(10);
  }

  noTone(speaker);

  delay(10);
}

void gameOver(bool win) {
  int extraPoints = 0;

  if (win) {
    switch (difficulty) {
      case 1:
        extraPoints = 500;
        break;
      case 2:
        extraPoints = 1000;
        break;
      case 3:
        extraPoints = 1500;
        break;
      case 4:
        extraPoints = 2000;
        break;
      default:
        extraPoints = 0;
        break;
    }

    int finalScore = (currentRound * 1000) + extraPoints;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Proficiat!!!");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(finalScore);

    for (int i = 0; i < 5; i++) {
      playNotes(1, 100);
      playNotes(2, 100);
      playNotes(3, 100);
      playNotes(4, 100);
    }
  } else {
     // lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Helaas...     ");
    delay(3000);
    lcd.setCursor(0, 0);

    lcd.print("Nog een keer?");
    for (int i = 0; i < 5; i++) {
      playNotes(1, 90);
      playNotes(2, 90);
      playNotes(3, 90);
      playNotes(4, 90);
    }
    // Behandeling voor verlies, zoals eerder gedefinieerd
    // ...
  }

  delay(3000);
  lcd.setCursor(0, 0);
  lcd.print("Druk een toets...");
  showMainMenu();
}
