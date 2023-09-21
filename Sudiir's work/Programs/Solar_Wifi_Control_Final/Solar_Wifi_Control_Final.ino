#include <WiFi.h>
#include <WebServer.h>
// #include <ESPAsyncWebServer.h>

const char* ssid = "BT-7274";
const char* password = "1029384756";

// Motor control pins
const int motor1PWMPin = 16;
const int motor1DirectionPin = 13;
const int motor2PWMPin = 18;
const int motor2DirectionPin = 17;
const int motor3PWMPin = 33;
const int motor3DirectionPin = 32;

// Relay control pins
const int relay1Pin = 26;
const int relay2Pin = 25;
const int relay3Pin = 4;

WebServer server(80);


void motorControlHandler(){
    int motorNum = server.arg("motor").toInt();
    int pwm = server.arg("pwm").toInt();
    int direction = server.arg("direction").toInt();
    Serial.println("Motor data");
    Serial.println(motorNum);
    Serial.println(pwm);
    Serial.println(direction);
    // Apply motor control based on motor number
    switch (motorNum) {
      case 1:
        analogWrite(motor1PWMPin, pwm);
        digitalWrite(motor1DirectionPin, direction);
        break;
      case 2:
        analogWrite(motor2PWMPin, pwm);
        digitalWrite(motor2DirectionPin, direction);
        break;
      case 3:
        analogWrite(motor3PWMPin, pwm);
        digitalWrite(motor3DirectionPin, direction);
        break;
      default:
        server.send(400, "text/plain", "Invalid motor number");
        return;
    }

    server.send(200, "text/plain", "Motor control applied");
  }


void relayHandler(){

    int relayNum = server.arg("relay").toInt();
    int state = server.arg("state").toInt();
    Serial.println("Relay data");
    Serial.println(relayNum);
    Serial.println(state);
    // Apply relay control based on relay number
    switch (relayNum) {
      case 1:
        digitalWrite(relay1Pin, state);
        break;
      case 2:
        digitalWrite(relay2Pin, state);
        break;
      case 3:
        digitalWrite(relay3Pin, state);
        break;
      default:
        server.send(400, "text/plain", "Invalid relay number");
        return;
    }

    server.send(200, "text/plain", "Relay control applied");
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

   pinMode(motor1PWMPin, OUTPUT);
  pinMode(motor1DirectionPin, OUTPUT);
  pinMode(motor2PWMPin, OUTPUT);
  pinMode(motor2DirectionPin, OUTPUT);
  pinMode(motor3PWMPin, OUTPUT);
  pinMode(motor3DirectionPin, OUTPUT);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);

  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(200, "text/plain", "ESP32 Motor and Relay Control");

  server.on("/motor",motorControlHandler);
  server.on("/relay", relayHandler);

  server.begin();
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}
