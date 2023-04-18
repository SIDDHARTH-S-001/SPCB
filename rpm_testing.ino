#define ENCA 32 //2 
#define ENCB 33 //3 
#define PWM  19 //6
#define DIR  18 //7

int pos = 0;
long prevT = 0;
float eprev = 0;
float eintegral = 0;

long pT = 0;
long cT = 0;
float rpm = 0;
int interval = 1000;
int prev_pos = 0;
int curr_pos = 0;
int diff = 0;

String x="";

void setup() {
  Serial.begin(9600);
  pinMode(ENCA,INPUT_PULLUP);
  pinMode(ENCB,INPUT);
  pinMode(PWM,OUTPUT);
  pinMode(DIR,OUTPUT);  
  attachInterrupt(digitalPinToInterrupt(ENCA),readEncoder,RISING);
  Serial.println("target pos");
  analogWrite(PWM, 127);
}

void loop() {
cT = millis();
if ((cT - pT) > interval){
  curr_pos = pos;
  diff = curr_pos - prev_pos;
  rpm = (float)((diff*60/7605)*36/39);  
  Serial.print("diff: ");
  Serial.print(diff);
  Serial.print(" rpm: ");
  Serial.println(rpm);
  pT = cT;
  prev_pos = curr_pos;
}
///
  delay(100);  
}

void readEncoder(){
  int b = digitalRead(ENCB);
  if(b == 1){
    pos++;
  }
  else{
    pos--;
  }
}
