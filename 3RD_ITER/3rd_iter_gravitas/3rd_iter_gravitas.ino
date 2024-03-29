#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

const char* ssid = "Micromax-HS2";
const char* password = "Micromax";

WiFiClient client;
WiFiServer server(80);
String data = "";

// Pins
// Motor driver 1 (right side)
#define pwm_drive 18
#define dir_drive 17
#define pwm_brush 16
#define dir_brush 13
// Motor driver 2 (left side)
#define lin_act_1_pwm 33  // both linear actuators working based on same signal
#define lin_act_1_dir 32
#define voltage_pin 14  // Can be used 21, 22 : Unused 12, 14, 5, 0, 2, 15
#define trig_1 21
#define echo_1 22
#define trig_2 23
#define echo_2 19
// #define lin_act_2_pwm 19
// #define lin_act_2_dir 23

// Variables
int rpm = 0;
int pos = 0;                   // linear acturator current position in (mm)
int new_pos = 0;               // Desired new position - set from APP (options: reference (20 mm), lift (30 mm), 
                               // clean (15 mm), deep clean (10 mm))
int dist = 0;                  // dist = difference between pos and new_pos  
int linear_actr_pwm = 255;     // pwm value (0-255)
float reference_volt = 11.0;   // reference voltage for which pwm 255 is calibrated to
float volt = 0.0;              // voltage reading from voltage sensor
float volt_scale_factor = 1.0; // voltage scale factor = V_multimeter/V_sensor
float duration_1 = 0.0;        // time of flight - ultrasonic sensor 1              
float duration_2 = 0.0;        // time of flight - ultrasonic sensor 2
int measurement_1 = 0;         // distance reading from ultrasonic sensor 1
int measurement_2 = 0;         // distance reading from ultrasonic sensor 2

void setup() {
  // put your setup code here, to run once:
  // Set Pinmodes
  pinMode(dir_drive, OUTPUT);
  pinMode(pwm_drive, OUTPUT);
  pinMode(dir_brush, OUTPUT);
  pinMode(pwm_brush, OUTPUT);
  pinMode(lin_act_1_pwm, OUTPUT);
  pinMode(lin_act_1_dir, OUTPUT);
  pinMode(voltage_pin, INPUT);
  pinMode(trig_1, OUTPUT);
  pinMode(echo_1, INPUT);
  pinMode(trig_2, OUTPUT);
  pinMode(echo_2, INPUT);
  // pinMode(lin_act_2_pwm, OUTPUT);
  // pinMode(lin_act_2_dir, OUTPUT);

  // Start Serial communication and WiFi connection
  Serial.begin(115200);
  Serial.println("Connecting to WIFI");
  WiFi.begin(ssid, password);
  while ((!(WiFi.status() == WL_CONNECTED))) {
    delay(500);
    Serial.print("...");
  }
  Serial.println("WiFi connected");
  Serial.println("ESP32 Local IP is : ");
  Serial.print((WiFi.localIP()));  // IP address of nodemcu (server) - should be enterer in the app (IP Textbox)
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  // int volt = analogRead(v_sensor);
  // float volt = map(analogRead(voltage_pin), 0, 4096, 0, 25);
  // volt = map(analogRead(voltage_pin), 0, 4095, 0, 25);
  // volt = volt * volt_scale_factor;
  volt = 13.3;
  linear_actr_pwm = calc_pwm(volt);

  measurement_1 = ultrasonic_1();
  measurement_2 = ultrasonic_2();

  // Serial.println(volt);

  client = server.available();
  if (!client) {
    return;
  }

  data = checkClient();
  String dataVal = data.substring(1, data.length());
  String rt = data.substring(0, 1);
  Serial.println(data);

  if (rt == "f") {
    Serial.println("Forward");
    rpm = dataVal.toInt();
    forward(rpm);
  }

  if (rt == "r") {
    Serial.println("Reverse");
    rpm = dataVal.toInt();
    reverse(rpm);
  }
  if (rt == "b") {
    Serial.println("Stop");
    brake();
  }

  if (rt == "c") {
    Serial.println("Brush Forward");
    rpm = dataVal.toInt();
    run_brush(rpm);
  }

  if (rt == "e") {
    Serial.println("Brush Reverse");
    rpm = dataVal.toInt();
    run_brush_rev(rpm);
  }

  if ((rt == "s" || measurement_1 > 30 || measurement_2 > 30)) {
    Serial.println("Stop Brush");
    stop_brush();
  }

  if (rt == "l") {
    Serial.println("Actuator moved to reference 2 cm stroke");
    new_pos = dataVal.toInt();  // 20
    if (pos > new_pos) {
      dist = pos - new_pos;
      move_actr_down(dist);
    } else {
      dist = new_pos - pos;
      move_actr_up(dist);
    }
  }

  if (rt == "m") {
    Serial.println("Actuator moved to 3 cm stroke: No Cleaning");
    new_pos = dataVal.toInt();  // 20
    if (pos > new_pos) {        // m30
      dist = pos - new_pos;
      move_actr_down(dist);
    } else {
      dist = new_pos - pos;
      move_actr_up(dist);
    }
  }

  if (rt == "n") {
    Serial.println("Actuator moved to 1.5 cm stroke: Mild Cleaning");
    new_pos = dataVal.toInt();  // 20
    if (pos > new_pos) {
      dist = pos - new_pos;
      move_actr_down(dist);
    } else {
      dist = new_pos - pos;
      move_actr_up(dist);
    }
  }

  if (rt == "o") {
    Serial.println("Actuator moved to 1 cm stroke: Deep Cleaning");
    new_pos = dataVal.toInt();  // 20
    if (pos > new_pos) {
      dist = pos - new_pos;
      move_actr_down(dist);
    } else {
      dist = new_pos - pos;
      move_actr_up(dist);
    }
  }

  if (rt == "p") {
    Serial.println("Linear Stop");
    lin_stop();
  }
  client.stop();
}

void forward(int rpm) {
  analogWrite(pwm_drive, rpm);
  digitalWrite(dir_drive, HIGH);

}

void reverse(int rpm) {
  analogWrite(pwm_drive, rpm);
  digitalWrite(dir_drive, LOW);
}

void brake() {
  analogWrite(pwm_drive, 0);
}

void run_brush(int rpm) {
  analogWrite(pwm_brush, rpm);
  digitalWrite(dir_brush, HIGH);
}

void run_brush_rev(int rpm) {
  analogWrite(pwm_brush, rpm);
  digitalWrite(dir_brush, LOW);
}

void stop_brush() {
  analogWrite(pwm_brush, 0);
}

void move_actr_up(int dist) {
  Serial.print("moving up ");
  pos = pos + dist;
  lin_inc(calc_time(dist));
}

void move_actr_down(int dist) {
  Serial.print("moving down ");
  pos = pos - dist;
  lin_dec(calc_time(dist));
}

void lin_inc(int dly) {
  analogWrite(lin_act_1_pwm, linear_actr_pwm);
  digitalWrite(lin_act_1_dir, LOW);
  delay(dly);
  lin_stop();
}

void lin_dec(int dly) {
  analogWrite(lin_act_1_pwm, linear_actr_pwm);
  digitalWrite(lin_act_1_dir, HIGH);
  delay(dly);
  lin_stop();
}

void lin_stop() {
  analogWrite(lin_act_1_pwm, 0);
}

int calc_time(int dist) {
  return (int)(1000 * dist / 6.893);
}

int calc_pwm(float volt) {
  // Calibration reference voltage = 13.34V, measured speed = 8.359 mm/s
  // implementation reference voltage = 11V, calculated proportional speed = 6.893 mm/s - corresponds to 255 PWM
  linear_actr_pwm = (11 / volt) * (255);
  return linear_actr_pwm;
}

int ultrasonic_1() {
    digitalWrite(trig_1, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_1, HIGH);
    delayMicroseconds(10);
    duration_1 = pulseIn(echo_1, HIGH);
    measurement_1 = (duration_1 / 2) * (0.034);
    Serial.print("Ultrasonic 1: ");
    Serial.print(measurement_1);
    Serial.println(" cm");
    return measurement_1;
}

int ultrasonic_2(){
    digitalWrite(trig_2, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_2, HIGH);
    delayMicroseconds(10);
    duration_2 = pulseIn(echo_2, HIGH);
    measurement_2 = (duration_2 / 2) * (0.034);
    Serial.print("Ultrasonic 2: ");
    Serial.print(measurement_2);
    Serial.println(" cm");
    return measurement_2;
}

String checkClient(void) {
  while (!client.available()) {
    delay(1);
    Serial.println(".");
  }
  Serial.println("Client Found");
  String request = client.readStringUntil('\r');  // http://{ nodemcu_local_IP }/{ route }{ value } | Eg:-  http://192.168.4.1/l30
  request.remove(0, 5);                           //    192.168.4.1/l30
  request.remove(request.length() - 9, 9);        // l30 = route value
  return request;
}