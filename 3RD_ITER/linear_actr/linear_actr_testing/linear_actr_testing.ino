#include <Preferences.h>

#define motor_pwm 12
#define motor_digital 14

// the current format for preference storage
// namespace sb-config {
//   dist: 0
// }

Preferences prefs; // currently at 0 on testing ESP32

const int freq = 5000;
const int pwmChannel = 0;
const int resolution = 8;

const int time_s = 3000;

int dist = 0;

/**
13.34v speed is 8.359mm/s
12v speed is 8.145mm/s (theoretical)
*/

int speed = 255;

void setup() {
  // put your setup code here, to run once:
  pinMode(motor_digital, OUTPUT);
  Serial.begin(115200);
  prefs.begin("sb-config");

  dist = prefs.getUInt("dist", 0);

  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(motor_pwm, pwmChannel);

  Serial.println("STARTING...");
  delay(3000);
  Serial.println("BEGINNING...");
  move_actuator_up(30); // change to const when checking
  move_actuator_down(10); // ^^
  Serial.println("STOPPING...");
  lin_actuator_stop();
  prefs.putUInt("dist", dist);
}

void setup_test() {
  pinMode(motor_digital, OUTPUT);
  Serial.begin(115200);
  prefs.begin("sb-config");

  calibrate();
  dist = prefs.getUInt("dist", 0);

  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(motor_pwm, pwmChannel);

  Serial.println("STARTING...");
  delay(3000);
  Serial.println("BEGINNING...");
  // move_actuator_up(30);
  move_actuator_down(dist); // should move to 0 pos as per data from memory
  Serial.println("STOPPING...");
  lin_actuator_stop();
  prefs.putUInt("dist", dist);
}

void loop() {

}

void move_actuator_up(uint distance) {
  Serial.println("HIGH");
  dist+=distance;
  lin_actuator_inc(calc_time(distance));
}

void move_actuator_down(uint distance) {
  Serial.println("LOW");
  dist-=distance;
  lin_actuator_dec(calc_time(distance));
}

int lin_actuator_inc(long delay) {
  ledcWrite(pwmChannel, speed);
  digitalWrite(motor_digital, HIGH);
  delay(delay); // dist in milli meter
  lin_actuator_stop();
}

int lin_actuator_dec(long delay) {
  ledcWrite(pwmChannel, speed);
  digitalWrite(motor_digital, LOW);
  delay(delay); // dist in milli meter
  lin_actuator_stop();
}

int lin_actuator_stop() {
  ledcWrite(pwmChannel, 0);
}

void calibrate() {
  lin_actuator_dec(15000); // Run LOW for 15s to reach lowest state at any case
  dist = 0;
}

long calc_time(int dist) {
  return (long) (1000 * dist / 8.359);
}