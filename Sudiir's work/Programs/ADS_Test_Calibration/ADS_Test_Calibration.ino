/**********************************************************************
    Solar Bot Bluetooth Controller
    
    AUTHOR : Sudiir Mohan
    VERSION : 0.1
**********************************************************************/


#include "ADS1X15.h"


#define ACS712_20A_SENSITIVITY 0.100  //     V/A
#define ACS712_OFFSET 0.15  // Volts


int16_t PV_Current = 0;  
int16_t PV_Voltage = 0;  
int16_t BATT_Voltage = 0;
int16_t Resistor_Divider [4] = {390,100,470,180};


ADS1115 ADS(0x48);   //Default I2C Address

void setup() {
  
  Serial.begin(115200);


  //Setting Up ADS
  ADS.begin();
  ADS.setGain(0);      // 6.144 volt
  ADS.setDataRate(7);  // fast
  ADS.setMode(0);      // continuous mode
  ADS.readADC(0);      // first read to trigger

}

void loop() {

  PV_Current = ADS.readADC(0);  
  PV_Voltage = ADS.readADC(1);  
  BATT_Voltage = ADS.readADC(2);   
  float f = ADS.toVoltage(1);  // voltage factor
 ADS.setGain(1);
  Serial.print("\tAnalog0: "); Serial.print(PV_Current); Serial.print('\t'); Serial.println((((PV_Current * f)-2.37)/ACS712_20A_SENSITIVITY-ACS712_OFFSET), 3);
ADS.setGain(0);  

  Serial.print("\tAnalog1: "); Serial.print(PV_Voltage); Serial.print('\t'); Serial.println(((PV_Voltage * f*(Resistor_Divider[0]+Resistor_Divider[1]))/Resistor_Divider[1])+0.130, 3); // less than 10v offset is -0.178
  Serial.print("\tAnalog2: "); Serial.print(BATT_Voltage); Serial.print('\t'); Serial.println(((BATT_Voltage * f*(Resistor_Divider[2]+Resistor_Divider[3]))/Resistor_Divider[3])+0.050, 3);  //less than 10 v offset is -0.155
  // Serial.print("\tAnalog3: "); Serial.print(val_3); Serial.print('\t'); Serial.println(val_3 * f, 3);
  //Serial.println(Resistor_Divider[0]);
  Serial.println();
 
  delay(250);

  

}

//        -- END OF PROGRAM --