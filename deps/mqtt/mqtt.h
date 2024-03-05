#ifndef MQTT_H
#define MQTT_H

#include "mqtt_client.h"

#define TAG "MQTT"

void mqtt_start(esp_mqtt_client_config_t mqtt_config1);
void mqtt_start_event(void (*mqtt_event_handler)(void *handler_args,
      esp_event_base_t base, int32_t event_id, void *event_data));
void log_error_if_nonzero(const char *message, int error_code);
void mqtt_envia_mensagem(char * topico, char * mensagem);
void mqtt_envia_mensagem_como(char * topico, char * mensagem, esp_mqtt_client_handle_t client1);

#endif
