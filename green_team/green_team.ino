#include "settings.h"

enum mainEngineState {
  STOP,
  FORWARD,
  BACKWARD,
};

enum turningEngineState {
  NONE,
  LEFT,
  RIGHT,
};

// MAIN ENGINE
mainEngineState currentMainEngineState = STOP;
int16_t currentMainSpeed = STOP_MAIN_ENGINE_SPEED;

void mainEngineControl(int16_t speedPwm, mainEngineState state) {
  switch (state)
  {
    case FORWARD:
      digitalWrite(MAIN_ENGINE_INPUT_1, LOW);
      digitalWrite(MAIN_ENGINE_INPUT_2, HIGH);
      break;

    case BACKWARD:
      digitalWrite(MAIN_ENGINE_INPUT_1, HIGH);
      digitalWrite(MAIN_ENGINE_INPUT_2, LOW);
      break;

    default:
      digitalWrite(MAIN_ENGINE_INPUT_1, LOW);
      digitalWrite(MAIN_ENGINE_INPUT_2, LOW);
      break;
  }
  
  delay(100);

  analogWrite(MAIN_ENGINE_ENABLE, speedPwm);
}

void increaseSpeed() {
  if (currentMainEngineState == BACKWARD) {
    if (currentMainSpeed >= MIN_MAIN_ENGINE_SPEED + MAIN_ENGINE_SPEED_STEP) {
      currentMainSpeed = currentMainSpeed - MAIN_ENGINE_SPEED_STEP;
      mainEngineControl(currentMainSpeed, currentMainEngineState);
      return;
    }

    if (currentMainSpeed == MIN_MAIN_ENGINE_SPEED) {
      currentMainEngineState == STOP;
      return;
    }
  }
  
  if (currentMainEngineState == STOP) {
    currentMainEngineState = FORWARD;
  }

  if (currentMainEngineState == FORWARD) {
    if (currentMainSpeed <= MAX_MAIN_ENGINE_SPEED - MAIN_ENGINE_SPEED_STEP) {
      currentMainSpeed = currentMainSpeed + MAIN_ENGINE_SPEED_STEP;
      mainEngineControl(currentMainSpeed, currentMainEngineState);
      return;
    }
  }
}

void decreaseSpeed() {
  if (currentMainEngineState == FORWARD) {
    if (currentMainSpeed >= MIN_MAIN_ENGINE_SPEED + MAIN_ENGINE_SPEED_STEP) {
      currentMainSpeed = currentMainSpeed - MAIN_ENGINE_SPEED_STEP;
      mainEngineControl(currentMainSpeed, currentMainEngineState);
      return;
    }

    if (currentMainSpeed == MIN_MAIN_ENGINE_SPEED) {
      currentMainEngineState == STOP;
      return;
    }
  }
  
  if (currentMainEngineState == STOP) {
    currentMainEngineState = BACKWARD;
  }

  if (currentMainEngineState == BACKWARD) {
    if (currentMainSpeed <= MAX_MAIN_ENGINE_SPEED - MAIN_ENGINE_SPEED_STEP) {
      currentMainSpeed = currentMainSpeed + MAIN_ENGINE_SPEED_STEP;
      mainEngineControl(currentMainSpeed, currentMainEngineState);
      return;
    }
  }
}

void stopMainEngine() {
  currentMainEngineState = STOP;
  currentMainSpeed = STOP_MAIN_ENGINE_SPEED;
  mainEngineControl(currentMainSpeed, currentMainEngineState);
}

// TURNING ENGINE
turningEngineState currentTurningEngineState = NONE;
int16_t currentTurningSpeed = STOP_TURNING_ENGINE_SPEED;

void turningEngineControl(int16_t speedPwm, turningEngineState state) {
  switch (state) {
    case LEFT:
      digitalWrite(TURNING_ENGINE_INPUT_1, LOW);
      digitalWrite(TURNING_ENGINE_INPUT_2, HIGH);
      break;

    case RIGHT:
      digitalWrite(TURNING_ENGINE_INPUT_1, HIGH);
      digitalWrite(TURNING_ENGINE_INPUT_2, LOW);
      break;

    default:
      digitalWrite(TURNING_ENGINE_INPUT_1, LOW);
      digitalWrite(TURNING_ENGINE_INPUT_2, LOW);
      break;
  }

  delay(100);

  analogWrite(TURNING_ENGINE_ENABLE, speedPwm);
}

void turnRight() {
  if (currentTurningEngineState == LEFT) {
    if (currentTurningSpeed >= MIN_TURNING_ENGINE_SPEED + TURNING_ENGINE_SPEED_STEP) {
      currentTurningSpeed = currentTurningSpeed - TURNING_ENGINE_SPEED_STEP;
      turningEngineControl(currentTurningSpeed, currentTurningEngineState);
      return;
    }

    if (currentTurningSpeed == MIN_TURNING_ENGINE_SPEED) {
      currentTurningEngineState == NONE;
      return;
    }
  }
  
  if (currentTurningEngineState == NONE) {
    currentTurningEngineState = RIGHT;
  }

  if (currentTurningEngineState == RIGHT) {
    if (currentTurningSpeed <= MAX_TURNING_ENGINE_SPEED - TURNING_ENGINE_SPEED_STEP) {
      currentTurningSpeed = currentTurningSpeed + TURNING_ENGINE_SPEED_STEP;
      turningEngineControl(currentTurningSpeed, currentTurningEngineState);
      return;
    }
  }
}

void turnLeft() {
  if (currentTurningEngineState == RIGHT) {
    if (currentTurningSpeed >= MIN_TURNING_ENGINE_SPEED + TURNING_ENGINE_SPEED_STEP) {
      currentTurningSpeed = currentTurningSpeed - TURNING_ENGINE_SPEED_STEP;
      turningEngineControl(currentTurningSpeed, currentTurningEngineState);
      return;
    }

    if (currentTurningSpeed == MIN_TURNING_ENGINE_SPEED) {
      currentTurningEngineState == NONE;
      return;
    }
  }
  
  if (currentTurningEngineState == NONE) {
    currentTurningEngineState = LEFT;
  }

  if (currentTurningEngineState == LEFT) {
    if (currentTurningSpeed <= MAX_TURNING_ENGINE_SPEED - TURNING_ENGINE_SPEED_STEP) {
      currentTurningSpeed = currentTurningSpeed + TURNING_ENGINE_SPEED_STEP;
      turningEngineControl(currentTurningSpeed, currentTurningEngineState);
      return;
    }
  }
}

void stopTurningEngine() {
  currentTurningEngineState = NONE;
  currentTurningSpeed = STOP_TURNING_ENGINE_SPEED;
  turningEngineControl(currentTurningSpeed, currentTurningEngineState);
}

// IR SENSOR
IRrecv irrecv(IR_SENSOR);
decode_results results;

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
        turnLeft();
        break;
        
      case IR_BTN_RIGHT:
        turnRight();
        break;
        
      case IR_BTN_STOP:
        stopMainEngine();
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

  pinMode(MAIN_ENGINE_ENABLE, OUTPUT);
  pinMode(MAIN_ENGINE_INPUT_1, OUTPUT);
  pinMode(MAIN_ENGINE_INPUT_2, OUTPUT);

  irrecv.enableIRIn();

  mainEngineControl(currentMainSpeed, currentMainEngineState);
}

void loop() {
  irControl();
}
