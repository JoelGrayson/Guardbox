#include <Servo.h>
Servo servo;

void setup() {
  Serial.begin(9600);
  servo.attach(9); //output to servo is 9

  lock();
  delay(3000);
  unlock();
}

void loop() {
  
}
void unlock() { //unlocks with servo motor
  servo.write(0);
}
void lock() { //locks with servo motor
  servo.write(95);
}
