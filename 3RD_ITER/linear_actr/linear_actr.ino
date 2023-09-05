#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

const char* ssid = "Micromax-HS2";
const char* password = "Micromax";

WiFiClient client;
WiFiServer server(80);
String data = "";

#define lin_act_pwm 34
#define lin_act_dir 39
#define voltage_pin 36

float speed = 0.0;
float stroke = 0.0;
float dist = 0;
float v_value = 0.0;
int p_time = 0;
int c_time = 0;
float pos = 0.0;

void setup() {
  // put your setup code here, to run once:
  pinMode(lin_act_pwm, OUTPUT);
  pinMode(lin_act_dir, OUTPUT);
  pinMode(voltage_pin, INPUT);

  Serial.begin(115200);
  Serial.println("Connecting to WIFI");
  WiFi.begin(ssid, password);
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(500);
    Serial.print("...");
  }
  Serial.println("WiFi connected");
  Serial.println("ESP32 Local IP is : ");
  Serial.print((WiFi.localIP()));
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
    float volt_value = map(analogRead(voltage_pin), 0, 4095, 0, 25);
    speed = 7 * (volt_value / 12);
    Serial.print("Voltage: ");
    Serial.println(volt_value);
    Serial.print("linear actuator position: ");
    Serial.println(pos);

    client = server.available();
    if (!client){
      return; 
      }
    data = checkClient();
    String dataVal = data.substring(1, data.length());
    String rt = data.substring(0, 1); 
    Serial.println(data); 

    if ((rt == "l")){
      Serial.println("Moving Linear Actuator Up");
      stroke = dataVal.toInt();
      dist = calc_lin_act_dist(speed);
      pos = pos + dist;
      float error = stroke - dist;
      while (error >= 0){
        lin_actuator_inc(stroke);
      }
      lin_actuator_stop();
    }

    else if ((rt == "m")){
      Serial.println("Moving Linear Actuator Down");
      stroke = dataVal.toInt();
      dist = calc_lin_act_dist(speed);
      pos = pos - dist;
      float error_opp = dist - stroke; 
      while (error_opp >= 0){
        lin_actuator_dec(stroke);
      }
      lin_actuator_stop();
    }

    else if ((rt == "n")){
      Serial.println("Stopping Linear Actuator (Failsafe)");
      lin_actuator_stop();
    }

}

String checkClient (void){
  while(!client.available()){
  delay(1); 
  Serial.println(".");}
  Serial.println("Client Found");
  String request = client.readStringUntil('\r');
  request.remove(0, 5);
  request.remove(request.length()-9,9);
  return request;
}

int lin_actuator_inc(int stroke_required){
    analogWrite(lin_act_pwm, 255);
    digitalWrite(lin_act_dir, HIGH);
}

int lin_actuator_dec(int stroke_required){
    analogWrite(lin_act_pwm, 255);
    digitalWrite(lin_act_dir, LOW);
}

int lin_actuator_stop(){
    analogWrite(lin_act_pwm, 0);
}

float calc_lin_act_dist(float speed){
  int c_time = millis();
  float stroke_covered = speed * (c_time - p_time);
  p_time = c_time;
  return stroke_covered;
}
