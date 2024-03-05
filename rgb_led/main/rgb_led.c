#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/ledc.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "wifi.h"
#include "mqtt.h"
#include "mqtt_handler.h"
#include "controller.h"
#include "my_nvs.h"
#include "buzzer.h"

#define RED_PIN     GPIO_NUM_23
#define GREEN_PIN   GPIO_NUM_22
#define BLUE_PIN    GPIO_NUM_21

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

void rgb_led_init() {
    ledc_timer_config_t timer_conf = {
        .duty_resolution = LEDC_TIMER_8_BIT, // Adjust as needed
        .freq_hz = 5000,   // PWM frequency
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t red_led_channel = {
        .channel    = LEDC_CHANNEL_0,
        .gpio_num   = RED_PIN,
        .speed_mode = LEDC_MODE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&red_led_channel);

    ledc_channel_config_t green_led_channel = {
        .channel    = LEDC_CHANNEL_1,
        .gpio_num   = GREEN_PIN,
        .speed_mode = LEDC_MODE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&green_led_channel);

    ledc_channel_config_t blue_led_channel = {
        .channel    = LEDC_CHANNEL_2,
        .gpio_num   = BLUE_PIN,
        .speed_mode = LEDC_MODE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&blue_led_channel);
}

// The RGB is inverted
// RED = 0 255 255
// GREEN = 255 0 255
// BLUE = 255 255 0
void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, red);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, green);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, blue);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);
}

void conectadoWifi(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      // Processamento Internet
      mqtt_start_event(mqtt_event_handler);
    }
  }
}

void app_main(void)
{
  nvs_init();
  rgb_led_init();
  conexaoWifiSemaphore = xSemaphoreCreateBinary();
  conexaoMQTTSemaphore = xSemaphoreCreateBinary();

  init_buzzer();

#ifdef EXAMPLE_POWER_SAVE_MODE
  ESP_LOGI("App", "POWER SAVE MODE ON");
  wifi_power_save();
#else
  ESP_LOGI("App", "POWER MODE OFF");
  wifi_start();
#endif
  vTaskDelay(pdMS_TO_TICKS(1000));
  xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 2048, NULL, 1, NULL);
  setup_state();
}
