#include "settings.h"
#include <Servo.h>

enum engineState {
  STOP,
  FORWARD,
  BACKWARD,
};

enum servoState {
  CENTER,
  LEFT,
  RIGHT,
};

// Engine related code
engineState mainEngineState = FORWARD;
int16_t mainSpeed = MIN_ENGINE_SPEED;

void engineControl(int16_t speedPwm, engineState state) {
  switch (state)
  {
    case FORWARD:
      digitalWrite(ENGINE_INPUT_1, LOW);
      digitalWrite(ENGINE_INPUT_2, HIGH);
      break;

    case BACKWARD:
      digitalWrite(ENGINE_INPUT_1, HIGH);
      digitalWrite(ENGINE_INPUT_2, LOW);
      break;

    default:
      digitalWrite(ENGINE_INPUT_1, LOW);
      digitalWrite(ENGINE_INPUT_2, LOW);
      break;
  }
  delay(100);

  analogWrite(ENGINE_ENABLE, mainSpeed);
}

// Servo related code
Servo servo;

servoState startServoState = CENTER;
int16_t currentServoDegrees = START_SERVO_DEGREES;

void servoControl(servoState state, Servo servo) {  
  switch(state) {
    case LEFT:
      if(currentServoDegrees <= MAX_SERVO_DEGREES - 5) {
        currentServoDegrees = currentServoDegrees + 5;
        servo.write(currentServoDegrees);
      }
      break;
      
    case RIGHT:
      if(currentServoDegrees >= MIN_SERVO_DEGREES + 5) {
        currentServoDegrees = currentServoDegrees - 5;
        servo.write(currentServoDegrees);
      }
      break;

    default:
      servo.write(START_SERVO_DEGREES);
      break;
  }

  delay(100);   
}

void increaseSpeed() {

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pinMode(ENGINE_ENABLE, OUTPUT);
  pinMode(ENGINE_INPUT_1, OUTPUT);
  pinMode(ENGINE_INPUT_2, OUTPUT);

  servo.attach(SERVO_INPUT);

  engineControl(mainSpeed, mainEngineState);
  servoControl(startServoState, servo);  
}

void loop() {
  // put your main code here, to run repeatedly:

  engineControl(600, mainEngineState);

  for(int i = 0; i < 30; i++) {
    // servoControl(LEFT, servo);
    Serial.println(currentServoDegrees);
  }

  for(int i = 0; i < 30; i++) {
    // servoControl(RIGHT, servo);
    Serial.println(currentServoDegrees);
  }
}
