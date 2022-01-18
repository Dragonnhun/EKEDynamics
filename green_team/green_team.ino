#include "settings.h"
#include <Servo.h>

enum engine_states {
  STOP,
  FORWARD,
  BACKWARDS
};

enum servo_states {
  CENTER;
  LEFT,
  RIGHT,
}

engine_states main_engine_state = STOP;
uint16_t main_speed = MIN_ENGINE_SPEED;

servo_states start_servo_state = CENTER;
uint16_t start_servo_degree = START_SERVO_DEGREE;
uint16_t curret_servo_degree = start_servo_degree;

void engine_control(uint16_t speed_pwm, engine_states state) {
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

void servo_control(uint16_t current_degrees, servo_states state, Servo myservo) {  
  switch(state) {
    case LEFT:
      if(current_degrees < 135 - 5) {
        current_degrees = current_degrees + 5;
        myservo.write(current_degrees);
      }
      break;
      
    case RIGHT:
      if(current_degrees > 45 + 5) {
        current_degrees = current_degrees - 5;
        myservo.write(current_degrees);
      }
      break;

    default:
      myservo.write(START_SERVO_DEGREE);
      break;
  }
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
  servo_control(START_SERVO_DEGREE, start_servo_state, myservo);
}

void loop() {
  // put your main code here, to run repeatedly:

}
