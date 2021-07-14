#include <Servo.h>
Servo servo;

#include <Key.h>
#include <Keypad.h>
const byte ROWS=4;
const byte COLS=4;

char hexaKeys[ROWS][COLS]={ //keypad symbols
  {'1', '2', '3', 'B'}, //B - backspace
  {'4', '5', '6', 'U'}, //U - unlock
  {'7', '8', '9', 'L'}, //L - lock
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS]={9, 8, 7, 6};
byte colPins[COLS]={5, 4, 3, 2};
Keypad keypadM=Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 24, 11, 28, 12, 22);

const byte codeLength=5;
char code[]={'1', '2', '3', '4', '*'};
byte userAttemptIndex;
char userAttempt[codeLength];
char lcdDisplay[codeLength]; //displayed to LCD (includes * for characters priorly typed to hide them)

const byte redLedPin=33;
const byte greenLedPin=35;
const byte buzzerPin=42;

void setup() {
  //Serial.begin(9600); //uncomment for debugging
  servo.attach(10); //output to servo is 10
  lcd.begin(16, 2);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  reset();
}

void loop() {
  char pressedKey=keypadM.getKey();
  if (pressedKey) { //keypad pressed
    Serial.print("Pressed: "); Serial.println(pressedKey);

    if (pressedKey=='B' && userAttemptIndex>0) { //backspace: delete last entry
        userAttemptIndex--;
        userAttempt[userAttemptIndex]='_';
        lcdDisplay[userAttemptIndex]='_';
        noise(2); //backspace keypressed noise
    } else if (userAttemptIndex<codeLength //if still '_' openings, enter code
      && pressedKey!='U' && pressedKey!='L' && pressedKey!='D') { //key not forbidden
      if (userAttemptIndex!=0) //hides prior letters with '*'
        lcdDisplay[userAttemptIndex-1]='*';
      lcdDisplay[userAttemptIndex]=pressedKey;
      userAttempt[userAttemptIndex]=pressedKey;
      userAttemptIndex++;
      noise(1); //keypressed noise
    }
    if (userAttemptIndex>=codeLength) { //all characters entered
      if (pressedKey=='U') { //unlock pressed
        bool correct=userCorrect();
        if (correct) {
          Serial.println("User correct");
          unlock();
          while (true) { //superloop until user locks door
            char pressedKey2=keypadM.getKey();
            digitalWrite(greenLedPin, HIGH);
            if (pressedKey2=='L') { //if lock pressed, lock & exit loop
              lock();
              reset();
              return;
            }
            delay(10); //for stability
          }
        } else {
          Serial.println("User wrong");
          noise(5);
          lock();
          reset();
        }
      }
    }
  }
  if (userAttemptIndex==0) { //no characters entered, so show instructions
    lcd.setCursor(0, 1);
    lcd.print("Enter code to unlock");
  }

  //Display on LCD
  lcd.clear();
  for (byte i=0; i<codeLength; i++) {
    lcd.print(lcdDisplay[i]); //NOTE: update to lcdDisplay[i]
  }
  if (userAttemptIndex==0) { //no characters entered, so show instructions
    lcd.setCursor(0, 1);
    lcd.print("Enter code");
  } else if (userAttemptIndex==5) { //all characters pressed
    lcd.setCursor(0, 1);
    lcd.print("Press unlock key");
  }

  delay(100); //for stability
}

void reset() {
  digitalWrite(greenLedPin, LOW); //turns off LEDs
  digitalWrite(redLedPin, LOW);
  userAttemptIndex=0;
  for (int i=0; i<codeLength; i++) { //fills in with '_'
    userAttempt[i]='_';
    lcdDisplay[i]='_';
  }
}

void unlock() { //unlocks with servo motor
  lcd.clear();
  noise(3);
  lcd.print("Unlocked");
  servo.write(0);
}

void lock() { //locks with servo motor
  lcd.clear();
  noise(4);
  lcd.print("Locked");
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, HIGH);
  servo.write(95);
  delay(1500);
  digitalWrite(redLedPin, LOW);
}

bool userCorrect() {
  for (byte i=0; i<codeLength; i++) {
    char attempt=userAttempt[i];
    char actual=code[i];
    if (attempt!=actual)
      return false; //if one mismatch, return false
  }
  return true; //otherwise return true
}
void noise(byte type) {
  switch (type) {
    case 1: tone(buzzerPin, 880, 150);  break; //key typed
    case 2: tone(buzzerPin, 320, 150);  break; //delete
    case 3: tone(buzzerPin, 1180, 400); break; //unlock/correct
    case 4: tone(buzzerPin, 400, 400);  break; //lock
    case 5: tone(buzzerPin, 148, 2000); break; //entered wrong
  }
}
