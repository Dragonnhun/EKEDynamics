#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>

#pragma region Constant_values

// L293D H-BRIDGE
#define MAIN_ENGINE_ENABLE D4
#define MAIN_ENGINE_INPUT_1 D5
#define MAIN_ENGINE_INPUT_2 D6

#define TURNING_ENGINE_ENABLE D1
#define TURNING_ENGINE_INPUT_1 D2
#define TURNING_ENGINE_INPUT_2 D0

// ULTRASONIC SENSOR
#define SOUND_VELOCITY 0.034

#define TRIG_PIN D8
#define ECHO_PIN D7

//#define TRIG_PIN_1 3
//#define ECHO_PIN_1 1

//#define SENSOR_LED 10

// IR SENSOR
#define IR_SENSOR D3

// SETTINGS
#define HOSTNAME "TheCar"
#define externalLight LED_BUILTIN

// MAIN ENGINE
#define MIN_MAIN_ENGINE_SPEED 700
#define MAX_MAIN_ENGINE_SPEED 1000
#define STOP_MAIN_ENGINE_SPEED 600
#define MAIN_ENGINE_SPEED_STEP 100

// TURNING ENGINE
#define MIN_TURNING_ENGINE_SPEED 500
#define MAX_TURNING_ENGINE_SPEED 1000
#define STOP_TURNING_ENGINE_SPEED 0
#define TURNING_ENGINE_SPEED_STEP 500

// IR SENSOR
#define IR_BTN_FORWARD 0xFF629D
#define IR_BTN_BACKWARD 0xFFA857
#define IR_BTN_LEFT 0xFF22DD
#define IR_BTN_RIGHT 0xFFC23D
#define IR_BTN_STOP 0xFF02FD
#define IR_BTN_SENSOR_TOGGLE 0xFF42BD
#define IR_BTN_TURNING_STOP 0xFF52AD

#pragma endregion

#pragma region Enum_definitions

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

#pragma endregion

#pragma region Global_variables

// IR SENSOR
IRrecv irrecv(IR_SENSOR);
decode_results results;

// ESP web server
ESP8266WebServer server(80);

// MAIN ENGINE
mainEngineState currentMainEngineState = STOP;
int16_t currentMainSpeed = STOP_MAIN_ENGINE_SPEED;

// TURNING ENGINE
turningEngineState currentTurningEngineState = NONE;
int16_t currentTurningSpeed = STOP_TURNING_ENGINE_SPEED;

float stopTrigerCm = 15;
float distanceCmBack;
//float distanceCmFront;
boolean ultrasonicIsEnabled = true; 

#pragma endregion

static const char WEB_CONTENT[] PROGMEM =  "<!DOCTYPE html> <html lang='en'> <head> <meta charset='UTF-8'> <meta http-equiv='X-UA-Compatible' content='IE=edge'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <title>NodeMcu Webes Fel??let</title> <script src=\"jquery-3.6.0.min.js\"></script> </head> <style> .top{ display: flex; justify-content: space-evenly; white-space: nowrap; } .top h3, h4{ display: inline-block; } .line-top{ border: 1px solid black; border-radius: 5px; margin-bottom: 4rem; } .line-bottom{ border: 1px solid black; border-radius: 5px; margin-top: 4rem; margin-bottom: 3rem; } .arrowbuttons { display: flex; justify-content: center; flex-wrap: wrap; } .container { align-items: center; display: flex; justify-content: center; padding: 30px 50px; border-radius: 12px; } .container:hover{ background-color: rgb(200, 200, 200); } .container__triangle { border-style: solid; height: 0px; width: 0px; } .container__triangle--up { border-color: transparent transparent rgba(0, 0, 0, 1); border-width: 0px 16px 16px; } .container__triangle--right { border-color: transparent transparent transparent rgba(0, 0, 0, 1); border-width: 16px 0px 16px 16px; } .container__triangle--down { border-color: rgba(0, 0, 0, 1) transparent transparent; border-width: 16px 16px 0px; } .container__triangle--left { border-color: transparent rgba(0, 0, 0, 1) transparent transparent; border-width: 16px 16px 16px 0px; } .uparrow { display: flex; justify-content: center; margin-bottom: 1rem; } .centerarrows{ display: flex; justify-content: center; } .downarrow{ display: flex; justify-content: center; margin-top: 1rem; } .bottom{ display: flex; justify-content: center; padding: 70px 0; } .bottombtn{ padding: 25px 50px; border-radius: 12px; font-size: 18px; font-weight: 700; margin: 0 35px; } .bottombtn:hover{ background-color: rgb(200, 200, 200); } </style> <body> <div class='top'> <div> <h3 style='margin-left: 10%; margin-right: 4px;'>Sensors:</h3> <h4 id='sensors' style='margin-right: 10%;'>ON</h4> </div> <div> <h3 style='margin-left: 10%; margin-right: 4px;'>Main Engine:</h3> <h4 id='engineM' style='margin-right: 10%;'>OFF</h4> </div> <div> <h3 style='margin-left: 10%; margin-right: 4px;'>Turning Engine:</h3> <h4 id='engineT' style='margin-right: 10%;'>OFF</h4> </div> <div> <h3 style='margin-right: 4px;'>Axis:</h3> <h3 id='axis' style='margin-right: 4px;'>0</h3> </div> <div> <h3 style='margin-right: 4px;'>Speed:</h3> <h3 id='speed' style='margin-right: 4px;'>0</h3> </div> </div> <hr class='line-top'> <div class='uparrow'> <button class='container' onclick='speedup()'> <div class='container__triangle container__triangle--up'></div> </button> </div> <div class='centerarrows'> <button class='container' onclick='axisdown()' style=\"margin: 0 70px;\"> <div class='container__triangle container__triangle--left'></div> </button> <button class='container' onclick='axisup()' style=\"margin: 0 70px;\"> <div class='container__triangle container__triangle--right'></div> </button> </div> <div class='downarrow'> <button class='container' onclick='speeddown()'> <div class='container__triangle container__triangle--down'></div> </button> </div> <div class=\"bottom\"> <button class='bottombtn' onclick='changeSensors()'>Sensors</button> <button class='bottombtn' onclick='changeEngineM()'>Main Engine</button> <button class='bottombtn' onclick='changeEngineT()'>Turning Engine</button> </div> </body> <script> function makeAjaxCall(url){$.ajax({\"url\": url})} function axisup(){ makeAjaxCall(\"right\"); var axis = document.getElementById('axis'); if(Number(axis.innerHTML) < 1000 ){ axis.innerHTML = Number(axis.innerHTML) + 100; } document.getElementById('axis').innerHTML = axis.innerHTML; document.getElementById('engineT').innerHTML = 'ON'; } function axisdown(){ makeAjaxCall(\"left\"); var axis = document.getElementById('axis'); if(Number(axis.innerHTML) > -1000 ){ axis.innerHTML = Number(axis.innerHTML) - 100; } document.getElementById('axis').innerHTML = axis.innerHTML; document.getElementById('engineT').innerHTML = 'ON'; } function speedup(){ makeAjaxCall(\"forward\"); var speed = document.getElementById('speed'); if(Number(speed.innerHTML) < 1000 ){ speed.innerHTML = Number(speed.innerHTML) + 100; } document.getElementById('speed').innerHTML = speed.innerHTML; document.getElementById('engineM').innerHTML = 'ON'; } function speeddown(){ makeAjaxCall(\"backward\"); var speed = document.getElementById('speed'); if(Number(speed.innerHTML) > -1000 ){ speed.innerHTML = Number(speed.innerHTML) - 100; } document.getElementById('speed').innerHTML = speed.innerHTML; document.getElementById('engineM').innerHTML = 'ON'; } function changeSensors() { makeAjaxCall(\"sesnors\"); var sensors = document.getElementById('sensors'); if (sensors.innerHTML == 'ON') sensors.innerHTML = 'OFF'; else sensors.innerHTML = 'ON'; } function changeEngineM() { makeAjaxCall(\"stop\"); document.getElementById('engineM').innerHTML = 'OFF'; document.getElementById('speed').innerHTML = 0; } function changeEngineT() { makeAjaxCall(\"turning-stop\"); document.getElementById('engineT').innerHTML = 'OFF'; document.getElementById('axis').innerHTML = 0; } </script> </html>";
