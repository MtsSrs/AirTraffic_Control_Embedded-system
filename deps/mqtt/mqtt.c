#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "mqtt.h"

extern SemaphoreHandle_t conexaoMQTTSemaphore;

esp_mqtt_client_handle_t client;
esp_mqtt_client_config_t mqtt_config = {
    .broker.address.uri = "mqtt://164.41.98.25",
    .credentials.username = "PgG5TcXjTCk8S6kGkIwW",
};

void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

void mqtt_start(esp_mqtt_client_config_t mqtt_config1)
{
    client = esp_mqtt_client_init(&mqtt_config1);
    esp_mqtt_client_start(client);
}

void mqtt_start_event(void (*mqtt_event_handler)(void *handler_args,
      esp_event_base_t base, int32_t event_id, void *event_data))
{
    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void mqtt_envia_mensagem(char * topico, char * mensagem)
{
    int message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
    ESP_LOGI(TAG, "Mensagem enviada, ID: %d", message_id);
}

void mqtt_envia_mensagem_como(char * topico, char * mensagem, esp_mqtt_client_handle_t client1)
{
    int message_id = esp_mqtt_client_publish(client1, topico, mensagem, 0, 1, 0);
    ESP_LOGI(TAG, "Mensagem enviada, ID: %d", message_id);
}

