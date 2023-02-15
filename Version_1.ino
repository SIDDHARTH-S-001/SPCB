#include<WiFi.h>

const char* ssid = "Micromax HS2";
const char* password = "Micromax";

WiFiClient client;
WiFiServer server(80);

String data = "";

int pwm_1 = 5;
int pwm_2 = 6;
int dir_1 = 8;
int dir_2 = 9;

int rpm = 0;

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
client = server.available();
    if (!client) 
    {
      return; 
    }
    data = checkClient();
    Serial.println(data);
    if (data.startsWith("f")) 
    {
      String dataVal = data.substring(1, data.length());
      Serial.println(dataVal);
      rpm = dataVal.toInt();
      analogWrite(pwm_1, rpm);
      analogWrite(pwm_2, rpm);
      digitalWrite(dir_1, HIGH);
      digitalWrite(dir_2, HIGH);
      Serial.println("moving_forward");
    }
    else if (data.startsWith("r")) 
    {
      String dataVal = data.substring(1, data.length());
      Serial.println(dataVal);
      rpm = dataVal.toInt();
      analogWrite(pwm_1, rpm);
      analogWrite(pwm_2, rpm);
      digitalWrite(dir_1, LOW);
      digitalWrite(dir_2, LOW);
      Serial.println("moving_reverse");
    }
    else if (data.startsWith("s")) 
    {
      String dataVal = data.substring(1, data.length());
      Serial.println(dataVal);
      rpm = dataVal.toInt();
      analogWrite(pwm_1, rpm);
      analogWrite(pwm_2, rpm);
      Serial.println("Stopping");
    }
      client.stop(); 
}

String checkClient (void)
{
  while(!client.available())
  {
  delay(1); 
  }
  String request = client.readStringUntil('\r');
  //request.body();
  // Serial.println(request);
  request.remove(0, 5);
  // Serial.println(request);
  request.remove(request.length()-9,9);
  // Serial.println(request);
  return request;
}
