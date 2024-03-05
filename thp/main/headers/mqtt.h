#ifndef MQTT_H
#define MQTT_H

#include "globals.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "freertos/queue.h"
#include "cJSON.h"
#include "mqtt_client.h"


void mqtt_start();

void mqtt_envia_mensagem(char *topico, char *mensagem);

void mqtt_disconnect();

void mqtt_reconnect();

#endif