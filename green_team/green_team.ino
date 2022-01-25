#include "settings.h"

#pragma region Engine_control

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
    if (currentMainSpeed >= MIN_MAIN_ENGINE_SPEED) {
      currentMainSpeed = currentMainSpeed - MAIN_ENGINE_SPEED_STEP;
      mainEngineControl(currentMainSpeed, currentMainEngineState);
      return;
    }

    if (currentMainSpeed <= MIN_MAIN_ENGINE_SPEED) {
      currentMainEngineState = STOP;
      return;
    }
  }

  if (currentMainEngineState == STOP) {
    currentMainEngineState = FORWARD;
  }

  if (currentMainEngineState == FORWARD) {
    if (currentMainSpeed < MAX_MAIN_ENGINE_SPEED) {
      currentMainSpeed = currentMainSpeed + MAIN_ENGINE_SPEED_STEP;
      mainEngineControl(currentMainSpeed, currentMainEngineState);
      return;
    }
  }
}

void decreaseSpeed() {
  if (currentMainEngineState == FORWARD) {
    if (currentMainSpeed >= MIN_MAIN_ENGINE_SPEED) {
      currentMainSpeed = currentMainSpeed - MAIN_ENGINE_SPEED_STEP;
      mainEngineControl(currentMainSpeed, currentMainEngineState);
      return;
    }

    if (currentMainSpeed <= MIN_MAIN_ENGINE_SPEED) {
      currentMainEngineState = STOP;
      return;
    }
  }

  if (currentMainEngineState == STOP) {
    currentMainEngineState = BACKWARD;
  }

  if (currentMainEngineState == BACKWARD) {
    if (currentMainSpeed < MAX_MAIN_ENGINE_SPEED) {
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

#pragma endregion

#pragma region Turning_engine

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
    if (currentTurningSpeed >= MIN_TURNING_ENGINE_SPEED) {
      currentTurningSpeed = currentTurningSpeed - TURNING_ENGINE_SPEED_STEP;
      turningEngineControl(currentTurningSpeed, currentTurningEngineState);
      return;
    }

    if (currentTurningSpeed < MIN_TURNING_ENGINE_SPEED) {
      currentTurningEngineState = NONE;
      return;
    }
  }

  if (currentTurningEngineState == NONE) {
    currentTurningEngineState = RIGHT;
  }

  if (currentTurningEngineState == RIGHT) {
    if (currentTurningSpeed < MAX_TURNING_ENGINE_SPEED) {
      currentTurningSpeed = currentTurningSpeed + TURNING_ENGINE_SPEED_STEP;
      turningEngineControl(currentTurningSpeed, currentTurningEngineState);
      return;
    }
  }
}

void turnLeft() {
  if (currentTurningEngineState == RIGHT) {
    if (currentTurningSpeed >= MIN_TURNING_ENGINE_SPEED) {
      currentTurningSpeed = currentTurningSpeed - TURNING_ENGINE_SPEED_STEP;
      turningEngineControl(currentTurningSpeed, currentTurningEngineState);
      return;
    }

    if (currentTurningSpeed < MIN_TURNING_ENGINE_SPEED) {
      currentTurningEngineState = NONE;
      return;
    }
  }

  if (currentTurningEngineState == NONE) {
    currentTurningEngineState = LEFT;
  }

  if (currentTurningEngineState == LEFT) {
    if (currentTurningSpeed < MAX_TURNING_ENGINE_SPEED) {
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

#pragma endregion

#pragma region Ir_controller

void irControl() {
  if (irrecv.decode(&results)) {
    serialPrintUint64(results.value, HEX);
    Serial.println();

    switch (results.value) {
      case IR_BTN_FORWARD:
        increaseSpeed();
        Serial.println(currentMainSpeed);
        Serial.println(currentMainEngineState);
        Serial.println();
        break;

      case IR_BTN_BACKWARD:
        decreaseSpeed();
        Serial.println(currentMainSpeed);
        Serial.println(currentMainEngineState);
        Serial.println();
        break;

      case IR_BTN_LEFT:
        turnLeft();
        Serial.println(currentTurningSpeed);
        Serial.println(currentTurningEngineState);
        Serial.println();
        break;

      case IR_BTN_RIGHT:
        turnRight();
        Serial.println(currentTurningSpeed);
        Serial.println(currentTurningEngineState);
        Serial.println();
        break;

      case IR_BTN_STOP:
        stopMainEngine();
        Serial.println(currentMainSpeed);
        Serial.println(currentMainEngineState);
        Serial.println();
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

#pragma endregion

#pragma Wifi_maneger

void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println("Wifi Manager");
  Serial.println("Please connect to AP");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println("To setup Wifi Configuration");
  flashLED(20, 50);
}

void flashLED(uint16_t number, uint16_t delayTime)
{
  for (int inx = 0; inx <= number; inx++)
  {
    delay(delayTime);
    digitalWrite(externalLight, LOW);  // ON
    delay(delayTime);
    digitalWrite(externalLight, HIGH);  // OFF
    delay(delayTime);
  }
}

#pragma endregion

#pragma region Web_interface

String getContent() {
  String content = FPSTR(WEB_CONTENT);
  return content;
}

void handleRoot() {
  server.send(200, "text/html", getContent());
}

#pragma endregion

void ultrasonicCheckDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  distanceCm = calculateDistance(duration);
  Serial.println("Distance:");
  Serial.println(distanceCm);
}

float calculateDistance(long duration) {
  return  duration * SOUND_VELOCITY/2;
}

void ultrasonicLoop() {
    if (ultrasinicIsEnabled) {
    ultrasonicCheckDistance();

    if (distanceCm <= stopTrigerCm && currentMainEngineState != STOP) {
      stopMainEngine();
      flashLED(20, 50);
    }
  }
}

void setup() {
  // Begin debug serial communication
  Serial.begin(115200);

  // SPIFFS file system initialization
  SPIFFS.begin();
  delay(200);

  // Set pin modes

  pinMode(MAIN_ENGINE_ENABLE, OUTPUT);
  pinMode(MAIN_ENGINE_INPUT_1, OUTPUT);
  pinMode(MAIN_ENGINE_INPUT_2, OUTPUT);

  pinMode(TURNING_ENGINE_ENABLE, OUTPUT);
  pinMode(TURNING_ENGINE_INPUT_1, OUTPUT);
  pinMode(TURNING_ENGINE_INPUT_2, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT); 
  pinMode(ECHO_PIN, INPUT);

  // Enable ir sensor
  irrecv.enableIRIn();

  // Set engines to basic state
  mainEngineControl(currentMainSpeed, currentMainEngineState);
  turningEngineControl(currentTurningSpeed, currentTurningEngineState);

  // Start wifi manager
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  if (!wifiManager.autoConnect((const char *) hostname.c_str()))
  {
    delay(3000);
    WiFi.disconnect(true);
    ESP.reset();
    delay(5000);
  }

  if (MDNS.begin(hostname)) {
    Serial.println("MDNS responder started");
  }

  // Start webserver

  server.serveStatic("/jquery-3.6.0.min.js", SPIFFS, "/jquery-3.6.0.min.js");

  server.on("/", handleRoot);
  server.on("/forward", []()
  {
    increaseSpeed();
    server.send(200, "text/plain", "next frame");
  });

  server.on("/backward", []()
  {
    decreaseSpeed();
    server.send(200, "text/plain", "next frame");
  });

  server.on("/stop", []()
  {
    stopMainEngine();
    server.send(200, "text/plain", "next frame");
  });

  server.on("/left", []()
  {
    turnLeft();
    server.send(200, "text/plain", "next frame");
  });

  server.on("/right", []()
  {
    turnRight();
    server.send(200, "text/plain", "next frame");
  });

  server.begin();
}

void loop() {
  irControl();
  server.handleClient();
  MDNS.update();
  ultrasonicLoop();
}
