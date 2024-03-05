#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "mqtt.h"
#include "controller.h"
#include "rgb_led.h"
#include "my_nvs.h"
#include "buzzer.h"

extern SemaphoreHandle_t conexaoMQTTSemaphore;

const int MAX_SLOTS = 5;
int current_slot = 1;
int slots = (1<<MAX_SLOTS);

esp_mqtt_client_handle_t client1;
esp_mqtt_client_config_t mqtt_config1 = {
    .broker.address.uri = "mqtt://164.41.98.25",
    .credentials.username = "l1WjJAakXigidmboNc2p",
};

typedef struct {
  int action;
} AirplaneOptions;

void set_airport_status(int status) {
  if(status) {
    // full
    mqtt_envia_mensagem("v1/devices/me/attributes", "{statusAirport: 1}");
  } else {
    mqtt_envia_mensagem("v1/devices/me/attributes", "{statusAirport: 0}");
  }
}

void runway_led_controller() {
  // set led to red
  set_rgb_color(0, 255, 255);
  mqtt_envia_mensagem("v1/devices/me/attributes", "{statusGreen: 0, statusRed: 1}");
  // wait 10 seconds
  vTaskDelay(pdMS_TO_TICKS(10000));
  // set status to green
  set_rgb_color(255, 0, 255);
  mqtt_envia_mensagem("v1/devices/me/attributes", "{statusRed: 0, statusGreen: 1}");
  xSemaphoreGive(conexaoMQTTSemaphore);
}

void buzzer_timer(void *params) {
  run_buzzer();
  vTaskDelay(pdMS_TO_TICKS(2000));
  stop_buzzer();
  vTaskDelete(NULL);
}

void airplane_add(void *params) {
  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
    if(current_slot > MAX_SLOTS) {
      ESP_LOGI(TAG, "Aeroporto está cheio.");
      xTaskCreate(&buzzer_timer, "Buzzer start", 2048, NULL, 1, NULL);
    } else {
      runway_led_controller();
      char json[10];
      ESP_LOGI(TAG, "Current slot: %d", current_slot);
      sprintf(json, "{led%d: 1}", current_slot);
      mqtt_envia_mensagem("v1/devices/me/attributes", json);
      current_slot++;
    }
  }
  if(current_slot == 6) {
    set_airport_status(1);
  }   
  nvs_write(slots, "slots");
  nvs_write(current_slot, "current_slot");
  xSemaphoreGive(conexaoMQTTSemaphore);
  vTaskDelete(NULL);
}

void airplane_remove(void *params) {
  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
    if(current_slot == 1) {
      ESP_LOGI(TAG, "Aeroporto já está vazio.");
      xTaskCreate(&buzzer_timer, "Buzzer start", 2048, NULL, 1, NULL);
    } else {
      runway_led_controller();
      current_slot--;
      char json[10];
      ESP_LOGI(TAG, "Current slot: %d", current_slot);
      sprintf(json, "{led%d: 0}", current_slot);
      mqtt_envia_mensagem("v1/devices/me/attributes", json);
    }
  }
  set_airport_status(0);
  nvs_write(slots, "slots");
  nvs_write(current_slot, "current_slot");
  xSemaphoreGive(conexaoMQTTSemaphore);
  vTaskDelete(NULL);
}

void setup_state() {
  client1 = esp_mqtt_client_init(&mqtt_config1);
  // set status to green
  set_rgb_color(255, 0, 255);
  mqtt_envia_mensagem("v1/devices/me/attributes", "{statusGreen: 1, statusRed: 0}");
  // Save a variable
  if(nvs_read("slots") > 0)
    slots = nvs_read("slots");
  if(nvs_read("current_slot") > 0)
    current_slot = nvs_read("current_slot");
}

void handle_embarque() {
  // add one plane
  AirplaneOptions op;
  op.action = 1;
  slots |= current_slot;
  xTaskCreate(&airplane_add, "Add airplane", 2048, NULL, 1, NULL);
}

void handle_desembarque() {
  // remove one plane
  AirplaneOptions op;
  op.action = 0;
  slots |= current_slot;
  xTaskCreate(&airplane_remove, "Remove airplane", 2048, NULL, 1, NULL);
}
