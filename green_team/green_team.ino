#include "settings.h"
#include <Servo.h>

enum engine_states {
  STOP,
  FORWARD,
  BACKWARDS
};

engine_states main_engine_state = STOP;
int main_speed = MIN_ENGINE_SPEED;

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

  analogWrite(ENGINE_EN, main_speed);
}

void increase_speed() {

}

Servo myservo;

void setup() {
  // put your setup code here, to run once:
  pinMode(ENGINE_EN, OUTPUT);
  pinMode(ENGINE_INPUT_1, OUTPUT);
  pinMode(ENGINE_INPUT_2, OUTPUT);

  myservo.attach(SERVO_INPUT);

  engine_control(main_speed, main_engine_state);

  myservo.write(START_SERVO_DEGREE); 
}

void loop() {
  // put your main code here, to run repeatedly:

}
