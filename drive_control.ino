#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

const char* ssid = "Micromax HS2";
const char* password = "Micromax";

WiFiClient client;
WiFiServer server(80);
String data = "";

#define pwm_1 19
#define dir_1 18
int sp = 0;
int rpm = 0;
int motor_pwm = 0;
int pwm = 0;
int diff = 0;

#define enc_1_A 32
#define enc_1_B 33

unsigned int count_pulses_1 = 0;
int gear_ratio = 18000/30;
int cpr = 30420; 
int ppr = cpr/4;
float prev_pos = 0;
float curr_pos = 0;
int pos = 0;
long prevT = 0;
int interval = 1000;
float dt = 0.01;

float kp = 0.0; // 1.0;
double kd = 0.0; // 0.025;
float ki = 0.0; // 0.0;
int e = 0;
float ed = 0.0;
float ei = 0.0;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(dir_1, OUTPUT);
  pinMode(pwm_1, OUTPUT);  
  
  pinMode(enc_1_A,INPUT_PULLUP); // sets the Encoder_output_A pin as the input
  pinMode(enc_1_B,INPUT); // sets the Encoder_output_B pin as the input
  attachInterrupt(digitalPinToInterrupt(enc_1_A),DC_Motor_Encoder_1,RISING);
  
  Serial.begin(115200);
  //Serial.println("Connecting to WIFI");
  WiFi.begin(ssid, password);
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(500);
    //Serial.print("...");
  }
  //Serial.println("WiFi connected");
  //Serial.println("ESP32 Local IP is : ");
  //Serial.print((WiFi.localIP()));
  server.begin();
}

void loop() {
  long currT = millis();
  // Serial.print(" count_pulse: ");
  // Serial.print(count_pulses_1);  
  int curr_pos = count_pulses_1;
  if ((currT - prevT)>1000){
    diff = curr_pos - prev_pos;
    rpm = (float)(diff*60/ppr);  
    motor_pwm = (rpm)*255/36;
    if (motor_pwm > 255){
      motor_pwm = 255;
    }  
    prevT = currT;
    prev_pos = curr_pos;
  }
  
//  Serial.print(" rpm: ");
//  Serial.print(rpm);
//  Serial.print(" pwm: ");
//  Serial.print(pwm);
//  Serial.print(" e: ");
    Serial.print("sp: ");
    Serial.print(sp);
    Serial.print(" motor_pwm: ");
    Serial.print(motor_pwm);
    Serial.print(" kp: ");
    Serial.print(kp);
    Serial.print(" ki: ");
    Serial.print(ki);
    Serial.print(" kd: ");
    Serial.println(kd);

  client = server.available();
    if (!client){
      return; 
      }
    data = checkClient();
    String dataVal = data.substring(1, data.length());
    String rt = data.substring(0, 1); 
    // Serial.println(data); 

    if (rt == "f"){
      // Serial.println("Full");
      sp = 230;
    }
    
    else if (rt == "h"){
      // Serial.println("Half");
      sp = 115;
    }
    
    else if ((rt == "t")){
      // Serial.println("One Third");
      sp = 77;
    }     

    else if ((rt == "s")){
      // Serial.println("Stop");
      brake();
    }
    
    else if ((rt == "p")){
    kp = kp + 0.1;
    // Serial.print("Increasing Kp by 0.5 , kp: ");
    // Serial.println(kp);
    }

    else if ((rt == "m")){
    kp = kp - 0.1;
    // Serial.print("Decreasing Kp by 0.5 , kp: ");
    // Serial.println(kp);
    }
    else if ((rt == "i")){
    ki = ki + 0.01;
    // Serial.print("Increasing Kp by 0.5 , kp: ");
    // Serial.println(kp);
    }

    else if ((rt == "n")){
    ki = ki - 0.01;
    // Serial.print("Decreasing Kp by 0.5 , kp: ");
    // Serial.println(kp);
    }
    else if ((rt == "x")){
    kd = kd + 0.005;
    // Serial.print("Increasing Kp by 0.5 , kp: ");
    // Serial.println(kp);
    }

    else if ((rt == "d")){
    kd = kd - 0.005;
    // Serial.print("Decreasing Kp by 0.5 , kp: ");
    // Serial.println(kp);
    }

    // Serial.print("SP: ");
    // Serial.println(sp);
    pid_control(sp, motor_pwm, kp, ki, kd);
    if (motor_pwm < 0){
      brake();
    }
    run_motor(motor_pwm);
    // Serial.println(motor_pwm);
    
    String response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    response += String(motor_pwm);
    client.print(response);
//    //Serial.println(response);    
}

String checkClient (void){
  while(!client.available())
  delay(1); 
  // Serial.println(".");}
  // Serial.println("Client Found");
  String request = client.readStringUntil('\r');
  request.remove(0, 5);
  request.remove(request.length()-9,9);
  return request;
}

int pid_control(int sp, int pwm, float kp, float ki, float kd){
  e = sp - motor_pwm;
  ed = e/dt;
  ei = e*dt;
  if (kp <= 0) {
   kd = 0;
   ki = 0;
  }
  kp = 0.5;
  ki = 0.02;
  kd = 0.02;
  motor_pwm = (kp * e) + (kd * ed) + (ki * ei);
  // motor_pwm = (kp * e);
  // Serial.println(motor_pwm);
  return motor_pwm;
}

void run_motor(int motor_pwm){
      analogWrite(pwm_1, motor_pwm);
      digitalWrite(dir_1, HIGH);
}

void brake(){
      analogWrite(pwm_1, 0);
      kp = 0;
      ki = 0;
      kd = 0;
      sp = 0;
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
