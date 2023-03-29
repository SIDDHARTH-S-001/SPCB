#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

const char* ssid = "Micromax HS2";
const char* password = "Micromax";

WiFiClient client;
WiFiServer server(80);
String data = "";

#define pwm_1 4
#define dir_1 23
#define pwm_2 13
#define dir_2 25
#define pwm_3 14
#define dir_3 17
int rpm = 0;

#define enc_1_A 5
#define enc_1_B 18
#define enc_2_A 35
#define enc_2_B 34
unsigned int count_pulses_1 = 0;
int count_pulses_2 = 0;
int count_avg = 0;
int gear_ratio = 18000/30;
int cpr = 30420; 
int ppr = cpr/4;
int wheel_circumference = 31; // value in cm
double total_distance = 0.0;
int total_displacement = 0;
double displacement=5;

#define trigpin_1 33
#define echopin_1 32
int height_1 = 0;
long duration_1 = 0.0;

#define trigpin_2 27
#define echopin_2 29
int height_2 = 0;
long duration_2 = 0.0;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(dir_1, OUTPUT);
  pinMode(dir_2, OUTPUT);
  pinMode(pwm_1, OUTPUT);
  pinMode(pwm_2, OUTPUT);
  pinMode(dir_3, OUTPUT);
  pinMode(pwm_3, OUTPUT);
  
  
  pinMode(enc_1_A,INPUT); // sets the Encoder_output_A pin as the input
  pinMode(enc_1_B,INPUT); // sets the Encoder_output_B pin as the input
  attachInterrupt(digitalPinToInterrupt(enc_1_A),DC_Motor_Encoder_1,RISING);
  pinMode(enc_2_A,INPUT); // sets the Encoder_output_A pin as the input
  pinMode(enc_2_B,INPUT); // sets the Encoder_output_B pin as the input
  attachInterrupt(digitalPinToInterrupt(enc_2_A),DC_Motor_Encoder_2,RISING);
  
  pinMode(trigpin_1,OUTPUT);
  pinMode(echopin_1,INPUT);
  pinMode(trigpin_2,OUTPUT);
  pinMode(echopin_2,INPUT);
  
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
//  digitalWrite(trigpin_1,LOW);
//  delayMicroseconds(2);
//  digitalWrite(trigpin_1,HIGH);
//  delayMicroseconds(10);
//  duration_1=pulseIn(echopin_1,HIGH);
//  height_1 =(duration_1/2)*(0.034);
//  //Serial.println(height_1);
//  //Serial.print(" cm");
//
//  digitalWrite(trigpin_2,LOW);
//  delayMicroseconds(2);
//  digitalWrite(trigpin_2,HIGH);
//  delayMicroseconds(10);
//  duration_2=pulseIn(echopin_2,HIGH);
//  height_2=(duration_2/2)*(0.034);
//  //Serial.println(height_2);
//  //Serial.print(" cm");
//
//  Serial.println(count_pulses_1);
//  Serial.println(count_pulses_2);
  int count_variation = count_pulses_1 - count_pulses_2;
//  Serial.println(count_variation);
  int displacement = calc_distance(count_pulses_1, count_pulses_2);
//  Serial.print("Total Distance: ");
//  displacement = ((count_pulses_1 * wheel_circumference)/ (30420 * 600));
//  displacement = count_pulses_1*1.7/1000000;
//  Serial.println(displacement);
  total_distance = total_distance + displacement;
//  Serial.println(total_displacement);
  
  client = server.available();
    if (!client){
      return; 
      }
    data = checkClient();
    String dataVal = data.substring(1, data.length());
    String rt = data.substring(0, 1); 
    Serial.println(data); 

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
    // else if ((rt == "b") || (height_1 >= 15) || (height_2 >= 15)){
    else if ((rt == "b")){
      Serial.println("Stop Bot");
      brake();
    }
     
    else if (rt == "c"){
      Serial.println("Run Brush");
      rpm = dataVal.toInt();
      run_brush(rpm);
    }

    else if ((rt == "s")){
      Serial.println("Stop Brush");
      stop_brush();
    }
    

//    Serial.println(count_pulses_1);
//    Serial.println(count_pulses_2);
    String response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    response += String(total_distance);
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
      analogWrite(pwm_1, rpm);
      digitalWrite(dir_1, HIGH);
      analogWrite(pwm_2, rpm);
      digitalWrite(dir_2, HIGH);
}

void reverse(int rpm){
      analogWrite(pwm_1, rpm);
      digitalWrite(dir_1, LOW);
      analogWrite(pwm_2, rpm);
      digitalWrite(dir_2, LOW);
}

void brake(){
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);
}

void run_brush(int rpm){
      analogWrite(pwm_3, rpm);
      digitalWrite(dir_3, HIGH);
}

void stop_brush(){
      analogWrite(pwm_3, 0);
}

void DC_Motor_Encoder_1(){
  int b = digitalRead(enc_1_B);
  if(b > 0){
    count_pulses_1 = count_pulses_1 + 1;
  }
  else{
    count_pulses_1 = count_pulses_1 - 1;
  }
}

void DC_Motor_Encoder_2(){
  int c = digitalRead(enc_2_B);
  if(c > 0){
    count_pulses_2 = count_pulses_2 + 1;
  }
  else{
    count_pulses_2 = count_pulses_2 - 1;
  }
}

int calc_distance(int count_1, int count_2){
  int count_value = (count_1/4) + (count_2/4);
  count_avg = (count_value)/2 ;
  //Serial.println(count_avg);
  // Distance traveled = (ticks ÷ (CPR × gear ratio)) × circumference of wheel
  int distance = (count_avg / (ppr * gear_ratio)) * wheel_circumference;
  //Serial.println(distance);
  return distance;
}
