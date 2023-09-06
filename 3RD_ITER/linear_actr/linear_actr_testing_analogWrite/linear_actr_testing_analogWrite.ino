#define pwm_pin 27
#define dir_pin 26

int dist = 0;

int speed_pwm = 255;

void setup() {
  // put your setup code here, to run once:
  pinMode(pwm_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);

  Serial.begin(115200);

  Serial.println("Starting ");
  delay(3000);
  Serial.println("Beginning");
  move_actr_up(30);
  move_actr_down(10);
  Serial.println("End ");

}

void loop() {
  // put your main code here, to run repeatedly:

}

void move_actr_up(int distance){
  Serial.print("moving up ");
  dist = dist + distance;
  lin_inc(calc_time(distance));
}

void move_actr_down(int distance){
  Serial.print("moving down ");
  dist = dist - distance;
  lin_dec(calc_time(distance));
}

void lin_inc(int dly){
  analogWrite(pwm_pin, speed_pwm);
  digitalWrite(dir_pin, HIGH);
  delay(dly);
  lin_stop();
}

void lin_dec(int dly){
  analogWrite(pwm_pin, speed_pwm);
  digitalWrite(dir_pin, LOW);
  delay(dly);
  lin_stop();
}

void lin_stop(){
  analogWrite(pwm_pin, 0);
}

int calc_time(int distance){
  return (int)(1000*distance/8.359);
}