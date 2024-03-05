#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
// #include "driver/adc.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
// #include "esp32/rom/uart.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "mqtt.h"

#define HIGH 1
#define LOW 0

#define DHT11_PIN GPIO_NUM_4

#define LED 15
#define BUZZER 18
#define DHT 17
#define BUTTON 19
#define HORIZONTAL 4
#define VERTICAL 16
#define LED_MODE 21
#define ESP_LED 2
// #define RAIN_LEVEL ADC1_CHANNEL_0

#define TAG_M "MQTT"
#define TAG_W "Wifi"

#define WIFI_SSID CONFIG_ESP_WIFI_SSID
#define WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define WIFI_MAXIMUM_RETRY CONFIG_ESP_MAXIMUM_RETRY

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

extern int full_power_mode;

extern SemaphoreHandle_t conexaoMQTTSemaphore;
extern SemaphoreHandle_t conexaoWifiSemaphore;

void wait_seconds(float seconds);

void grava_valor_nvs(int32_t valor);

int32_t le_valor_nvs();

#endif