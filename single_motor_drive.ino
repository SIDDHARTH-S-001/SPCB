#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

const char* ssid = "Micromax-HS2";
const char* password = "Micromax";

WiFiClient client;
WiFiServer server(80);
String data = "";

#define pwm_drive 4
#define dir_drive 0
#define enc_A 39
#define enc_B 36
#define pwm_brush 2
#define dir_brush 15
#define lin_act_pwm 17
#define lin_act_in1 18
#define lin_act_in2 19
#define voltage_pin 25

int rpm = 0;
float speed = 0;
float stroke = 0;
float dist = 0;
float displacement = 0.0;

int count_pulses = 0;
int gear_ratio = 18000/30;
int cpr = 30420; 
int ppr = cpr/4;
float wheel_circumference = 2*3.141593*5; // value in cm
int total_distance = 0;
int total_displacement = 0;
int p_time = 0;
int c_time = 0;

int v_sensor = 0;
float v_value = 0.0;

void setup() {
  // put your setup code here, to run once:
  pinMode(dir_drive, OUTPUT);
  pinMode(pwm_drive, OUTPUT);
  pinMode(dir_brush, OUTPUT);
  pinMode(pwm_brush, OUTPUT);
  pinMode(lin_act_pwm, OUTPUT);
  pinMode(lin_act_in1, OUTPUT);
  pinMode(lin_act_in2, OUTPUT);
  
  pinMode(enc_A,INPUT_PULLUP); // sets the Encoder_output_A pin as the input
  pinMode(enc_B,INPUT_PULLUP); // sets the Encoder_output_B pin as the input
  attachInterrupt(digitalPinToInterrupt(enc_A),DC_Motor_Encoder, RISING);
  
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
    // int volt = analogRead(v_sensor);
    float volt_value = map(analogRead(v_sensor), 0, 4096, 0, 25);
    speed = 7 * (volt_value / 12);

    client = server.available();
    if (!client){
      return; 
      }
    data = checkClient();
    String dataVal = data.substring(1, data.length());
    String rt = data.substring(0, 1); 
    Serial.println(data); 

    displacement = calc_distance(count_pulses);

    if (rt == "f"){
      Serial.println("Forward");
      rpm = dataVal.toInt();
      forward(rpm);
      total_displacement = total_displacement + displacement;
    }
    else if (rt == "r"){
      Serial.println("Reverse");
      rpm = dataVal.toInt();
      reverse(rpm);
      total_displacement = total_displacement - displacement;
    }
    else if (rt == "s"){
      Serial.println("Stop");
      brake();
    }

    else if (rt == "c"){
      Serial.println("Brush Forward");
      rpm = dataVal.toInt();
      run_brush(rpm);
    }

    else if (rt == "e"){
      Serial.println("Brush Reverse");
      rpm = dataVal.toInt();
      run_brush_rev(rpm);
    }

    else if ((rt == "s")){
      Serial.println("Stop Brush");
      stop_brush();
    }

    else if ((rt == "l")){
      Serial.println("Moving Linear Actuator Up");
      stroke = dataVal.toInt();
      dist = calc_lin_act_dist(speed);
      float error = stroke - dist;
      while (error <= 0){
        lin_actuator_inc(stroke);
      }
      lin_actuator_stop();
    }

    else if ((rt == "m")){
      Serial.println("Moving Linear Actuator Down");
      stroke = dataVal.toInt();
      dist = calc_lin_act_dist(speed);
      float error_opp = dist - stroke; 
      while (error_opp <= 0){
        lin_actuator_dec(stroke);
      }
      lin_actuator_stop();
    }

    else if ((rt == "n")){
      Serial.println("Stopping Linear Actuator (Failsafe)");
      lin_actuator_stop();
    }

    String response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    response += String(total_displacement);
    client.print(response);
    Serial.println(response);    

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

void forward(int rpm){
      analogWrite(pwm_drive, rpm);
      digitalWrite(dir_drive, HIGH);
}

void reverse(int rpm){
      analogWrite(pwm_drive, rpm);
      digitalWrite(dir_drive, LOW);
}

void brake(){
      analogWrite(pwm_drive, 0);
}

void run_brush(int rpm){
      analogWrite(pwm_brush, rpm);
      digitalWrite(dir_brush, HIGH);
}

void run_brush_rev(int rpm){
      analogWrite(pwm_brush, rpm);
      digitalWrite(dir_brush, LOW);
}

void stop_brush(){
      analogWrite(pwm_brush, 0);
}

int lin_actuator_inc(int stroke_required){
    analogWrite(lin_act_pwm, 255);
    digitalWrite(lin_act_in1, HIGH);
    digitalWrite(lin_act_in2, LOW);
}

int lin_actuator_dec(int stroke_required){
    analogWrite(lin_act_pwm, 255);
    digitalWrite(lin_act_in1, LOW);
    digitalWrite(lin_act_in2, HIGH);
}

int lin_actuator_stop(){
    analogWrite(lin_act_pwm, 0);
}

void DC_Motor_Encoder(){
  int b = digitalRead(enc_B);
  if(b > 0){
    count_pulses = count_pulses + 1;
  }
  else{
    count_pulses = count_pulses - 1;
  }
}

float calc_distance(int count){
  // Distance traveled = (ticks ÷ (CPR × gear ratio)) × circumference of wheel
  float distance = (count / (ppr * gear_ratio)) * wheel_circumference;
  // Serial.println(distance);
  return distance;
}

float calc_lin_act_dist(float speed){
  int c_time = millis();
  float stroke_covered = speed * (c_time - p_time);
  p_time = c_time;
  return stroke_covered;
}