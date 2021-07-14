#include <Servo.h>
Servo servo;

#include <Key.h>
#include <Keypad.h>
const byte ROWS=4;
const byte COLS=4;
char hexaKeys[ROWS][COLS]={ //keypad symbols
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS]={9, 8, 7, 6};
byte colPins[COLS]={5, 4, 3, 2};
Keypad keypadM=Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int codeLength=5;
char code[]={'1', '2', '3', '4', '*'};
int userAttemptIndex=0;
char userAttempt[]={'0', '0', '0', '0', '0'};
boolean locked=true;

void setup() {
  Serial.begin(9600);
  servo.attach(10); //output to servo is 10
  lock();
}

void loop() {
  char customKey=keypadM.getKey();
  if (customKey) {
    Serial.print("Pressed: "); Serial.println(customKey);
    userAttempt[userAttemptIndex]=customKey;
    userAttemptIndex++;
    if (userAttemptIndex==codeLength) { //all keys entered
      boolean userCorrect=true;
      for (int i=0; i<codeLength; i++) {
        char attempt=userAttempt[i];
        char actual=code[i];
        if (attempt!=actual)
          userCorrect=false;
      }
      if (userCorrect) {
        Serial.println("User correct");
        unlock();
      } else {
        Serial.println("wrong");
        lock();
      }
    }
  }
}
void unlock() { //unlocks with servo motor
  locked=false;
  Serial.println("unlocking");
  servo.write(0);
}
void lock() { //locks with servo motor
  locked=true;
  Serial.println("locking");
  servo.write(95);
}
