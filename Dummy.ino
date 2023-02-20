#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#define pwm_1 4
#define dir_1 0
#define pwm_2 2
#define dir_2 15

int rpm = 0;
const char* ssid = "Micromax HS2";
const char* password = "Micromax";

WiFiClient client;
WiFiServer server(80);

String data = "";

void setup() {
  // put your setup code here, to run once:
  
  pinMode(dir_1, OUTPUT);
  pinMode(dir_2, OUTPUT);
  pinMode(pwm_1, OUTPUT);
  pinMode(pwm_2, OUTPUT);
  Serial.begin(115200);

  Serial.println("Connecting to WIFI");
  WiFi.begin(ssid, password);
  while ((!(WiFi.status() == WL_CONNECTED)))
  {
    delay(500);
    Serial.print("..");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("NodeMCU Local IP is : ");
  Serial.print((WiFi.localIP()));
  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("Loop Started");
  client = server.available();
    if (!client) 
    {
      return; 
      }
    data = checkClient();
    // Serial.println(data);
    String dataVal = data.substring(1, data.length());
    String rt = data.substring(0, 1); 
      Serial.println(data); 

    if (rt == "f"){
      Serial.println("Forward");
      rpm = dataVal.toInt();
      analogWrite(pwm_1, rpm);
      analogWrite(pwm_2, rpm);
      digitalWrite(dir_1, HIGH);
      digitalWrite(dir_2, HIGH);
    }
    else if (rt == "r"){
      Serial.println("Reverse");
      rpm = dataVal.toInt();
      analogWrite(pwm_1, rpm);
      analogWrite(pwm_2, rpm);
      digitalWrite(dir_1, LOW);
      digitalWrite(dir_2, LOW);
    }
    else if (rt == "s"){
      Serial.println("Stop");
      rpm = dataVal.toInt();
      
      analogWrite(pwm_1, rpm);
      analogWrite(pwm_2, rpm);
    }
}

String checkClient (void)
{
  while(!client.available())
  {
  delay(1); 
  Serial.println(".");}
  Serial.println("Client Found");
  String request = client.readStringUntil('\r');
  //request.body();
  // Serial.println(request);
  request.remove(0, 5);
  // Serial.println(request);
  request.remove(request.length()-9,9);
  // Serial.println(request);
  return request;
}
