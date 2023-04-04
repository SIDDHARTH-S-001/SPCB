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
float dt = 0;
int motor_speed = 0;

#define enc_1_A 36
#define enc_1_B 39

unsigned int count_pulses_1 = 0;
int gear_ratio = 18000/30;
int cpr = 30420; 
int ppr = cpr/4;
float prev_pos = 0;
int pos = 0;
long prevT = 0;

float kp = 1;
float kd = 0.025;
float ki = 0.0;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(dir_1, OUTPUT);
  pinMode(pwm_1, OUTPUT);  
  
  pinMode(enc_1_A,INPUT); // sets the Encoder_output_A pin as the input
  pinMode(enc_1_B,INPUT); // sets the Encoder_output_B pin as the input
  attachInterrupt(digitalPinToInterrupt(enc_1_A),DC_Motor_Encoder_1,RISING);
  
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
  
  long currT = millis();
  float deltaT = ((float) (currT - prevT));
  int curr_pos = count_pulses_1*360/cpr;
  dt = deltaT/1000;
  int rpm = (curr_pos - prev_pos)/deltaT;
  prevT = currT;
  prev_pos = curr_pos;

  client = server.available();
    if (!client){
      return; 
      }
    data = checkClient();
    String dataVal = data.substring(1, data.length());
    String rt = data.substring(0, 1); 
    Serial.println(data); 

    if (rt == "f"){
      Serial.println("Full");
      sp = dataVal.toInt();
    }
    
    else if (rt == "h"){
      Serial.println("Half");
      sp = dataVal.toInt();
    }
    
    else if ((rt == "t")){
      Serial.println("One Third");
    }     

    else if ((rt == "s")){
      Serial.println("Stop");
      brake();
    }

    pid_control(sp);
    
    String response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    response += String(motor_speed);
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

int pid_control(int sp){
  int e = sp - rpm;
  float ed = e/dt;
  float ei = e*dt;
  motor_speed = (kp * e) + (kd * ed) + (ki * ei);
  return motor_speed;
}

void brake(){
      analogWrite(pwm_1, 0);
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
