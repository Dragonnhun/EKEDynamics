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

ESP8266WebServer server(80);

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

// IR SENSOR
IRrecv irrecv(IR_SENSOR);
decode_results results;

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

void configModeCallback(WiFiManager *myWiFiManager);

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

String getContent() {
  return "<!DOCTYPE html><html lang='en'><head> <meta charset='UTF-8'> <meta http-equiv='X-UA-Compatible' content='IE=edge'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <title>HtmlMinta</title> <script src=\"jquery.min.js\"></script></head><style>.top{display: flex; justify-content: space-evenly; white-space: nowrap;}.top h3, h4{display: inline-block;}.line-top{border: 1px solid black; border-radius: 5px; margin-bottom: 4rem;}.line-bottom{border: 1px solid black; border-radius: 5px; margin-top: 4rem; margin-bottom: 3rem;}.arrowbuttons{display: flex; justify-content: center; flex-wrap: wrap;}.container{align-items: center; display: flex; justify-content: center; padding: 30px 50px; border-radius: 12px;}.container:hover{background-color: rgb(200, 200, 200);}.container__triangle{border-style: solid; height: 0px; width: 0px;}.container__triangle--up{border-color: transparent transparent rgba(0, 0, 0, 1); border-width: 0px 16px 16px;}.container__triangle--right{border-color: transparent transparent transparent rgba(0, 0, 0, 1); border-width: 16px 0px 16px 16px;}.container__triangle--down{border-color: rgba(0, 0, 0, 1) transparent transparent; border-width: 16px 16px 0px;}.container__triangle--left{border-color: transparent rgba(0, 0, 0, 1) transparent transparent; border-width: 16px 16px 16px 0px;}.uparrow{display: flex; justify-content: center; margin-bottom: 2.5rem;}.centerarrows{display: flex; justify-content: center;}.downarrow{display: flex; justify-content: center; margin-top: 2.5rem;}.bottom{display: flex; justify-content: center; padding-bottom: 50px;}.bottombtn{padding: 25px 50px; border-radius: 12px; font-size: 18px; font-weight: 700;}.bottombtn:hover{background-color: rgb(200, 200, 200);}</style><body> <div class='top'> <div class='asd'> <h3 style='margin-left: 10%; margin-right: 4px;'>Sensors:</h3> <h4 id='sensors' style='margin-right: 10%;'>ON</h4> </div><div> <h3 style='margin-right: 4px;'>Lights:</h3> <h4 id='lights' style='margin-right: 10%;'>OFF</h4> </div><div> <h3 style='margin-right: 4px;'>Axis:</h3> <h3 id='axis' style='margin-right: 4px;'>0</h3> <h4 style='margin-right: 10%;'>°</h4> </div><div> <h3 style='margin-right: 4px;'>Speed:</h3> <h3 id='speed' style='margin-right: 4px;'>0</h3> <h4 style='margin-right: 10%;'>%</h4> </div></div><hr class='line-top'> <div class='uparrow'> <button class='container' onclick='speedup()'> <div class='container__triangle container__triangle--up'></div></button> </div><div class='centerarrows'> <button class='container' onclick='axisdown()'> <div class='container__triangle container__triangle--left'></div></button> <button class='container' style='margin: 0px 50px' onclick='speedstop()'> <h2>STOP</h2> </button> <button class='container' onclick='axisup()'> <div class='container__triangle container__triangle--right'></div></button> </div><div class='downarrow'> <button class='container' onclick='speeddown()'> <div class='container__triangle container__triangle--down'></div></button> </div><hr class='line-bottom'> <div class='bottom'> <button class='bottombtn' onclick='changeSensors()' style='margin-right: 75px;'>Sensors</button> <button class='bottombtn' onclick='changeLights()' style='margin-left: 75px;'>Lights</button> </div></body><script>function axisup(){var axis=document.getElementById('axis'); if(Number(axis.innerHTML) < 90 ){axis.innerHTML=Number(axis.innerHTML) + 10;}document.getElementById('axis').innerHTML=axis.innerHTML;}function axisdown(){var axis=document.getElementById('axis'); if(Number(axis.innerHTML) > -90 ){axis.innerHTML=Number(axis.innerHTML) - 10;}document.getElementById('axis').innerHTML=axis.innerHTML;}function speedup(){var speed=document.getElementById('speed'); if(Number(speed.innerHTML) < 100 ){speed.innerHTML=Number(speed.innerHTML) + 10;}document.getElementById('speed').innerHTML=speed.innerHTML; makeAjaxCall(\"forward\");}function speeddown(){var speed=document.getElementById('speed'); if(Number(speed.innerHTML) > 0 ){speed.innerHTML=Number(speed.innerHTML) - 10;}document.getElementById('speed').innerHTML=speed.innerHTML;}function speedstop(){document.getElementById('speed').innerHTML=0;}function changeSensors(){var sensors=document.getElementById('sensors'); if (sensors.innerHTML=='ON') sensors.innerHTML='OFF'; else sensors.innerHTML='ON';}function changeLights(){var lights=document.getElementById('lights'); if (lights.innerHTML=='ON') lights.innerHTML='OFF'; else lights.innerHTML='ON';}</script></html>";
}

void handleRoot() {
  server.send(200, "text/html", getContent());
  //flashLED(20, 50);
}

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  delay(200);

  pinMode(MAIN_ENGINE_ENABLE, OUTPUT);
  pinMode(MAIN_ENGINE_INPUT_1, OUTPUT);
  pinMode(MAIN_ENGINE_INPUT_2, OUTPUT);

  pinMode(TURNING_ENGINE_ENABLE, OUTPUT);
  pinMode(TURNING_ENGINE_INPUT_1, OUTPUT);
  pinMode(TURNING_ENGINE_INPUT_2, OUTPUT);

  irrecv.enableIRIn();

  mainEngineControl(currentMainSpeed, currentMainEngineState);
  turningEngineControl(currentTurningSpeed, currentTurningEngineState);
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

  server.serveStatic("/jquery-3.6.0.min.js", SPIFFS, "/jquery-3.6.0.min.js");

  server.on("/", handleRoot);
  server.on("/forward", increaseSpeed);
  server.on("/backward", decreaseSpeed);
  server.on("/stop", stopMainEngine);
  server.on("/left", turnLeft);
  server.on("/right", turnRight);
  server.begin();
}

void loop() {
  irControl();
  server.handleClient();
  MDNS.update();
}
