/**********************************************************************
    Solar Bot Controller
    
    AUTHOR : Sudiir Mohan
    VERSION : 0.1
	STATUS : ALPHA
**********************************************************************/
#include <stdio.h>
#include <WebServer.h>
#include <ADS1X15.h>
#include "esp_wifi.h"
#include "esp_bt.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "driver/rtc_io.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "driver/rtc_io.h"
#include "driver/rtc_cntl.h"
#include "esp32/rom/rtc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define I2C_SCA 21
#define I2C_SDL 22
#define EXTRA_RELAY 27
#define ADS1115_ALRT 35
#define ADS1115_ADDR 34
#define LIMIT1 39
#define LIMIT2 36


#define ACS712_20A_SENSITIVITY 0.100  //     V/A
#define ACS712_OFFSET 0.15  // Volts

#define TIME_TO_SLEEP 10

static const char* TAG = "MAIN";
static bool motorRunning = false;
static bool solarPanelOn = false;
static bool diagnosticsMode = false;
static bool motorDirection = false;


volatile int16_t PV_Current = 0;  
volatile int16_t PV_Voltage = 0;  
volatile int16_t BATT_Voltage = 0;
int16_t Resistor_Divider [4] = {390,100,470,180};


const char* ssid = "BT-7274";
const char* password = "1029384756";

// Motor control pins
const int motor1PWMPin = 16;
const int motor1DirectionPin = 13;
const int motor2PWMPin = 18;
const int motor2DirectionPin = 17;
const int motor3PWMPin = 33;
const int motor3DirectionPin = 32;

// Relay control pins
const int relay1Pin = 26;
const int relay2Pin = 25;
const int relay3Pin = 4;


WebServer server(80);
ADS1115 ADS(0x48);   //Default I2C Address

void enterAutoSleepMode() {
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * 1000000); // Convert to microseconds
        printf("Entering deep sleep for %d seconds...\n", TIME_TO_SLEEP);
        esp_err = esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
        if (esp_err == ESP_OK) {
            esp_err = esp_light_sleep_start();
        }
        if (esp_err != ESP_OK) {
            printf("Error (%d) while entering deep sleep.\n", esp_err);
        }
}


void updateRTC() {
    time_t now;
    struct tm timeinfo;
	time(&now);
    localtime_r(&now, &timeinfo);
    rtc_time_set(&timeinfo);
    ESP_LOGI(TAG, "RTC time updated to %04d-%02d-%02d %02d:%02d:%02d",
    timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
    timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}


float updateADCValues() {
	PV_Current = ADS.readADC(0);  
	PV_Voltage = ADS.readADC(1);  
	BATT_Voltage = ADS.readADC(2);   
	float f = ADS.toVoltage(1);  // voltage factor
	ADS.setGain(1);
	//Serial.print("\tAnalog0: "); Serial.print(PV_Current); Serial.print('\t'); Serial.println((((PV_Current * f)-2.37)/ACS712_20A_SENSITIVITY-ACS712_OFFSET), 3);
	ADS.setGain(0);  

	//Serial.print("\tAnalog1: "); Serial.print(PV_Voltage); Serial.print('\t'); Serial.println(((PV_Voltage * f*(Resistor_Divider[0]+Resistor_Divider[1]))/Resistor_Divider[1])+0.130, 3); // less than 10v offset is -0.178
	//Serial.print("\tAnalog2: "); Serial.print(BATT_Voltage); Serial.print('\t'); Serial.println(((BATT_Voltage * f*(Resistor_Divider[2]+Resistor_Divider[3]))/Resistor_Divider[3])+0.050, 3);  //less than 10 v offset is -0.155
	// Serial.print("\tAnalog3: "); Serial.print(val_3); Serial.print('\t'); Serial.println(val_3 * f, 3);
	//Serial.println(Resistor_Divider[0]);
	//Serial.println();
	
}

void controlSolarChargingPWM(bool enable) {
	// Not developed yet
}


void enableDiagnosticsMode() {
	// For very Future development
    diagnosticsMode = true;
}


void logDataToSDCard() {
   // Not Developed Yet
}


void motorControlHandler(){
    int motorNum = server.arg("motor").toInt();
    int pwm = server.arg("pwm").toInt();
    int direction = server.arg("direction").toInt();
    Serial.println("Motor data");
    Serial.println(motorNum);
    Serial.println(pwm);
    Serial.println(direction);
    // Apply motor control based on motor number
    switch (motorNum) {
      case 1:
        analogWrite(motor1PWMPin, pwm);
        digitalWrite(motor1DirectionPin, direction);
        break;
      case 2:
        analogWrite(motor2PWMPin, pwm);
        digitalWrite(motor2DirectionPin, direction);
        break;
      case 3:
        analogWrite(motor3PWMPin, pwm);
        digitalWrite(motor3DirectionPin, direction);
        break;
      default:
        server.send(400, "text/plain", "Invalid motor number");
        return;
    }

    server.send(200, "text/plain", "Motor control applied");
  }


void relayHandler(){

    int relayNum = server.arg("relay").toInt();
    int state = server.arg("state").toInt();
    Serial.println("Relay data");
    Serial.println(relayNum);
    Serial.println(state);
    // Apply relay control based on relay number
    switch (relayNum) {
      case 1:
        digitalWrite(relay1Pin, state);
        break;
      case 2:
        digitalWrite(relay2Pin, state);
        break;
      case 3:
        digitalWrite(relay3Pin, state);
        break;
      default:
        server.send(400, "text/plain", "Invalid relay number");
        return;
    }

    server.send(200, "text/plain", "Relay control applied");
}

void IRAM_ATTR gpio_isr_handler(void* arg) {
    if (motorDirection == FALSE)
    motorDirection = true;
	else 
	motorDirection = FALSE;
}

void app_main() {
	esp_err_t esp_err;
	

	gpio_config_t io_conf1, io_conf2;
    io_conf1.pin_bit_mask = (1ULL << LIMIT1);
    io_conf1.mode = GPIO_MODE_INPUT;
    io_conf1.intr_type = EDGE_TRIGGER;
    io_conf1.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf1.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf1);
	io_conf2.pin_bit_mask = (1ULL << LIMIT2);
    io_conf2.mode = GPIO_MODE_INPUT;
    io_conf2.intr_type = EDGE_TRIGGER;
    io_conf2.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf2.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf2);
	
	gpio_install_isr_service(0);
	
	esp_err = rtc_gpio_init(GPIO_NUM_0);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize RTC GPIO: %s", esp_err_to_name(esp_err));
        vTaskDelay(portMAX_DELAY);
    }
    // Configure and enable the RTC peripheral
    rtc_config_t rtc_cfg = {};
    rtc_cfg.clk_src = RTC_SLOW_MEM_CLK;
    rtc_cfg.counter_en = true;
    rtc_cfg.counter_mode = RTC_COUNT_UP;
    rtc_cfg.alarm_en = false;
    rtc_cfg.counter_high = 0;
    rtc_cfg.counter_low = 0;
    esp_err = rtc_configure(&rtc_cfg);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure RTC: %s", esp_err_to_name(esp_err));
        vTaskDelay(portMAX_DELAY);
    }

    // Configure and enable automatic deep sleep after 24 hours (86400 seconds)
    esp_err = esp_sleep_enable_timer_wakeup(86400 * 1000000);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable deep sleep timer: %s", esp_err_to_name(esp_err));
        vTaskDelay(portMAX_DELAY);
	
	
   	// Initialize serial communication
	Serial.begin(115200);

	// Connect to Wi-Fi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
	delay(1000);
	Serial.println("Connecting to WiFi...");
	}
	Serial.println("Connected to WiFi");
	Serial.print("Local IP: ");
	Serial.println(WiFi.localIP());
	

	pinMode(motor1PWMPin, OUTPUT);
	pinMode(motor1DirectionPin, OUTPUT);
	pinMode(motor2PWMPin, OUTPUT);
	pinMode(motor2DirectionPin, OUTPUT);
	pinMode(motor3PWMPin, OUTPUT);
	pinMode(motor3DirectionPin, OUTPUT);
	pinMode(relay1Pin, OUTPUT);
	pinMode(relay2Pin, OUTPUT);
	pinMode(relay3Pin, OUTPUT);
	
	
	ADS.begin();
	ADS.setGain(0);      // 6.144 volt
	ADS.setDataRate(7);  // fast
	ADS.setMode(0);      // continuous mode
	ADS.readADC(0);      // first read to trigger

	
	// server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
	// request->send(200, "text/plain", "ESP32 Motor and Relay Control");

	server.on("/motor",motorControlHandler);
	server.on("/relay", relayHandler);

	server.begin();

    xTaskCreate(motorControlBluetooth, "motor_bt_task", 2048, NULL, 5, NULL);
    xTaskCreate(edgeDetectionTask, "edge_detection_task", 2048, NULL, 5, NULL);
    xTaskCreate(logDataToSDCard, "sd_card_logging_task", 4096, NULL, 5, NULL);

    while (1) {
		
		server.handleClient();
		updateRtcTime();
	}
}


