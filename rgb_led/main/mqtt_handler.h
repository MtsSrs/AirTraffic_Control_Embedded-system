#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include "mqtt_client.h"

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

#endif // MQTT_HANDLER_H
