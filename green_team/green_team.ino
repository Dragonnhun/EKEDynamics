#include "settings.h"
#include <Servo.h>

enum engine_states {
  STOP,
  FORWARD,
  BACKWARDS
};

engine_states main_engine_state = STOP;
int main_speed = MIN_ENGINE_SPEED;
int prev_main_speed = MIN_ENGINE_SPEED;

void engine_control(int speed_pwm, engine_states state) {
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

void increase_speed() {
  if (main_speed == 0)
  {
    main_speed = MIN_ENGINE_SPEED;
  }
  else if (main_speed < MAX_ENGINE_SPEED)
  {
    main_speed = main_speed + SPEED_STEP;
  } 
  else if (main_speed < 0 && main_speed > -MAX_ENGINE_SPEED)
  {
    main_speed = 0;
  }
  Serial.println(main_speed);
}

void decrease_speed() {
  
}

Servo myservo;

void setup() {
  Serial.begin(115200);
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
increase_speed();
delay(200);
}
