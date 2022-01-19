#include "settings.h"

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

// ENGINE
engineState currentEngineState = STOP;
int16_t currentSpeed = STOP_ENGINE_SPEED;
IRrecv irrecv(IR_SENSOR);
decode_results results;

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

  analogWrite(ENGINE_ENABLE, speedPwm);
}

void increaseSpeed() {
  if (currentEngineState == BACKWARD) {
    if (currentSpeed >= MIN_ENGINE_SPEED + SPEED_STEP) {
      currentSpeed = currentSpeed - SPEED_STEP;
      engineControl(currentSpeed, currentEngineState);
      return;
    }

    if (currentSpeed == MIN_ENGINE_SPEED) {
      currentEngineState == STOP;
      return;
    }
  }
  
  if (currentEngineState == STOP) {
    currentEngineState = FORWARD;
  }

  if (currentEngineState == FORWARD) {
    if (currentSpeed <= MAX_ENGINE_SPEED - SPEED_STEP) {
      currentSpeed = currentSpeed + SPEED_STEP;
      engineControl(currentSpeed, currentEngineState);
      return;
    }
  }
}

void decreaseSpeed() {
  if (currentEngineState == FORWARD) {
    if (currentSpeed >= MIN_ENGINE_SPEED + SPEED_STEP) {
      currentSpeed = currentSpeed - SPEED_STEP;
      engineControl(currentSpeed, currentEngineState);
      return;
    }

    if (currentSpeed == MIN_ENGINE_SPEED) {
      currentEngineState == STOP;
      return;
    }
  }
  
  if (currentEngineState == STOP) {
    currentEngineState = BACKWARD;
  }

  if (currentEngineState == BACKWARD) {
    if (currentSpeed <= MAX_ENGINE_SPEED - SPEED_STEP) {
      currentSpeed = currentSpeed + SPEED_STEP;
      engineControl(currentSpeed, currentEngineState);
      return;
    }
  }
}

void stopEngine() {
  currentEngineState = STOP;
  currentSpeed = STOP_ENGINE_SPEED;
  engineControl(currentSpeed, currentEngineState);
}

// SERVO
Servo servo;

servoState currentServoState = CENTER;
int16_t currentServoDegrees = START_SERVO_DEGREES;

void servoControl(servoState state) {
  switch (state) {
    case LEFT:
      if (currentServoDegrees <= MAX_SERVO_DEGREES - 5) {
        currentServoDegrees = currentServoDegrees + 5;
        servo.write(currentServoDegrees);
      }
      break;

    case RIGHT:
      if (currentServoDegrees >= MIN_SERVO_DEGREES + 5) {
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

// IR SENSOR
void irControl() {
  if (irrecv.decode(&results)) {
    serialPrintUint64(results.value, HEX);
    
    switch (results.value) {
      case IR_BTN_FORWARD:
        increaseSpeed();
        break;
        
      case IR_BTN_BACKWARD:
        decreaseSpeed();
        break;
        
      case IR_BTN_LEFT:
        servoControl(LEFT);
        break;
        
      case IR_BTN_RIGHT:
        servoControl(RIGHT);
        break;
        
      case IR_BTN_STOP:
        stopEngine();
        break;
        
      case IR_BTN_SENSOR_TOGGLE:
        // Sensor toggle method call...
        break;
        
      default:
        break;
    }
    
    irrecv.resume();
  }
  
  delay(100);
}

void setup() {
  Serial.begin(115200);

  pinMode(ENGINE_ENABLE, OUTPUT);
  pinMode(ENGINE_INPUT_1, OUTPUT);
  pinMode(ENGINE_INPUT_2, OUTPUT);

  servo.attach(SERVO_INPUT);
  irrecv.enableIRIn();

  engineControl(currentSpeed, currentEngineState);
  servoControl(currentServoState);
}

void loop() {
  irControl();
}
