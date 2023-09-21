/**********************************************************************
    Solar Bot WiFi Controller
    
    AUTHOR : Sudiir Mohan
    VERSION : 1.0
**********************************************************************/

#include<WiFi.h>
#include "ADS1X15.h"

//----------------------------PINS-------------------------------------
#define MD1_DIR1 13
#define MD1_PWM1 16
#define MD1_DIR2 17
#define MD1_PWM2 18
#define MD2_DIR1 32
#define MD2_PWM1 33
#define MD2_DIR2 23
#define MD2_PWM2 19
#define I2C_SCA 21
#define I2C_SDL 22
#define MD1_RELAY 26
#define MD2_RELAY 25
#define PV_RELAY 4
#define EXTRA_RELAY 27
#define ADS1115_ALRT 35
#define ADS1115_ADDR 34
#define LIMIT1 39
#define LIMIT2 36

//---------------------------Constant Values------------------------------------
#define ACS712_20A_SENSITIVITY 0.100  //     V/A
#define ACS712_OFFSET 0.15  // Volts



//--------------------------Values----------------------------------------------
const char* ssid = "BT-7274";
const char* password = "1029384756";
volatile int16_t PV_Current = 0;  
volatile int16_t PV_Voltage = 0;  
volatile int16_t BATT_Voltage = 0;
int16_t Resistor_Divider [4] = {390,100,470,180};


//-------------------------Object Declarations------------------------------------
ADS1115 ADS(0x48);


void setup() {
  

  Serial.begin(115200);

  //Setting Pin Registers
  pinMode(MD1_DIR1,OUTPUT);
  pinMode(MD1_PWM1,OUTPUT);
  pinMode(MD1_DIR2,OUTPUT);
  pinMode(MD1_PWM1,OUTPUT);
  pinMode(MD2_DIR1,OUTPUT);
  pinMode(MD2_PWM1,OUTPUT);
  pinMode(MD2_DIR2,OUTPUT);
  pinMode(MD2_PWM1,OUTPUT);
  pinMode(MD1_RELAY,OUTPUT);
  pinMode(MD2_RELAY,OUTPUT);
  pinMode(PV_RELAY,OUTPUT);
  pinMode(EXTRA_RELAY,OUTPUT);
  pinMode(ADS1115_ALRT,INPUT);
  // pinMode(ADS1115_ADDR,HIGH);
  pinMode(LIMIT1,INPUT);
  pinMode(LIMIT2,INPUT);

  
  //Setting Up ADS
  ADS.begin();
  ADS.setGain(0);      // 6.144 volt
  ADS.setDataRate(7);  // fast
  ADS.setMode(0);      // continuous mode
  ADS.readADC(0);      // first read to trigger


}





void loop() {

  

}





/*
 * Global variables
 */