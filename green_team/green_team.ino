#include "settings.h"
#include <Servo.h>

enum engine_states {
  STOP,
  FORWARD,
  BACKWARDS
};

engine_states main_engine_state = FORWARD;
uint8_t speed = 70;

void engine_control(uint8_t speed_pwm, engine_states state) {
  switch (state)
  {
    case FORWARD:
      digitalWrite(ENGINE_INPUT_1, LOW);
      digitalWrite(ENGINE_INPUT_2, HIGH);
      break;

    case BACKWARDS:
      digitalWrite(ENGINE_INPUT_1, HIGH);
      digitalWrite(ENGINE_INPUT_2, LOW);
      break;

    default:
      digitalWrite(ENGINE_INPUT_1, LOW);
      digitalWrite(ENGINE_INPUT_2, LOW);
      break;
  }
  delay(100);

  analogWrite(ENGINE_EN, speed_pwm);
}

Servo myservo;

void setup() {
  // put your setup code here, to run once:
  pinMode(ENGINE_EN, OUTPUT);
  pinMode(ENGINE_INPUT_1, OUTPUT);
  pinMode(ENGINE_INPUT_2, OUTPUT);

  myservo.attach(SERVO_INPUT);

  engine_control(speed, STOP);

  myservo.write(45); 
}

void loop() {
  // put your main code here, to run repeatedly:

}
