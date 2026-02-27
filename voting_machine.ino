#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD (I2C address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Candidate button pins
const int candidate1ButtonPin = 13;
const int candidate2ButtonPin = 14;
const int candidate3ButtonPin = 27;

// Control buttons
const int resetButtonPin = 32;
const int showResultsButtonPin = 33;

// Vote counters
int candidate1Votes = 0;
int candidate2Votes = 0;
int candidate3Votes = 0;

// Debounce variables for candidate buttons
bool lastButtonState1 = HIGH;
bool lastButtonState2 = HIGH;
bool lastButtonState3 = HIGH;

unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;

// Debounce variables for control buttons
bool lastResetButtonState = HIGH;
bool lastShowResultsButtonState = HIGH;

unsigned long lastResetDebounceTime = 0;
unsigned long lastShowResultsDebounceTime = 0;

const unsigned long debounceDelay = 50; // milliseconds

// Election state
bool electionEnded = false;

void setup() {
  Serial.begin(115200);

  // Setup candidate buttons
  pinMode(candidate1ButtonPin, INPUT_PULLUP);
  pinMode(candidate2ButtonPin, INPUT_PULLUP);
  pinMode(candidate3ButtonPin, INPUT_PULLUP);

  // Setup control buttons
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(showResultsButtonPin, INPUT_PULLUP);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Welcome message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voting Machine");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voting in progress...");
}

void loop() {
  checkResetButton();
  checkShowResultsButton();

  if (!electionEnded) {
    checkButton(candidate1ButtonPin, candidate1Votes, lastButtonState1, lastDebounceTime1, "C1");
    checkButton(candidate2ButtonPin, candidate2Votes, lastButtonState2, lastDebounceTime2, "C2");
    checkButton(candidate3ButtonPin, candidate3Votes, lastButtonState3, lastDebounceTime3, "C3");
  }

  delay(10);
}

// Function to handle candidate button
void checkButton(int pin, int &voteCount, bool &lastButtonState,
                 unsigned long &lastDebounceTime, String candidateName) {

  int reading = digitalRead(pin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW) {
      voteCount++;
      Serial.print(candidateName);
      Serial.print(" Votes: ");
      Serial.println(voteCount);

      showVoteConfirmation(candidateName);
      delay(200);
    }
  }

  lastButtonState = reading;
}

// Vote confirmation display
void showVoteConfirmation(String candidateName) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voted: ");
  lcd.print(candidateName);

  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voting in progress...");
}

// Reset election
void checkResetButton() {
  int reading = digitalRead(resetButtonPin);

  if (reading != lastResetButtonState) {
    lastResetDebounceTime = millis();
  }

  if ((millis() - lastResetDebounceTime) > debounceDelay) {
    if (reading == LOW) {
      candidate1Votes = 0;
      candidate2Votes = 0;
      candidate3Votes = 0;
      electionEnded = false;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Election Reset");
      delay(1500);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Voting in progress...");
    }
  }

  lastResetButtonState = reading;
}

// Show results and end election
void checkShowResultsButton() {
  int reading = digitalRead(showResultsButtonPin);

  if (reading != lastShowResultsButtonState) {
    lastShowResultsDebounceTime = millis();
  }

  if ((millis() - lastShowResultsDebounceTime) > debounceDelay) {
    if (reading == LOW) {
      electionEnded = true;
      updateDisplay();
    }
  }

  lastShowResultsButtonState = reading;
}

// Display final vote counts
void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C1:");
  lcd.print(candidate1Votes);
  lcd.print(" C2:");
  lcd.print(candidate2Votes);

  lcd.setCursor(0, 1);
  lcd.print("C3:");
  lcd.print(candidate3Votes);
}