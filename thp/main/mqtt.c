#include "mqtt.h"
#include "cJSON.h"
#include "esp_log.h"

static esp_mqtt_client_handle_t client;
int powerSaving = 0;

void handle_response(char *data)
{
    cJSON *root = cJSON_Parse(data);
  
    cJSON *method = cJSON_GetObjectItem(root, "method");
    if (method == NULL) {
        ESP_LOGE(TAG_M, "Method not found in JSON");
        cJSON_Delete(root);
        return;
    }

    if (!strcmp(method->valuestring, "lowPower"))
    {
        cJSON *params = cJSON_GetObjectItem(root, "params");
        if (params == NULL) {
            ESP_LOGE(TAG_M, "Params not found in JSON");
            cJSON_Delete(root);
            return;
        }

        cJSON *lowPower = cJSON_GetObjectItem(params, "lowPower");
        if (lowPower == NULL) {
            ESP_LOGE(TAG_M, "lowPower not found in JSON");
            cJSON_Delete(root);
            return;
        }

        powerSaving = lowPower->valueint;
        printf("PowerSaving = %d\n", powerSaving);
    }

    cJSON_Delete(root);
}


static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{

    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG_M, "MQTT_EVENT_CONNECTED");

        msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 0);
        // msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 0);
        // msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/attributes", 0);
        xSemaphoreGive(conexaoMQTTSemaphore);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG_M, "MQTT_EVENT_DISCONNECTED");
        xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY);
        xSemaphoreGive(conexaoWifiSemaphore);
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG_M, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG_M, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG_M, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG_M, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        handle_response(event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG_M, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG_M, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG_M, "Event dispatched from event loop base=%s, event_id=%d", base, (int)event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_start()
{
    // while (xSemaphoreTake(conexaoMQTTSemaphore, 2) == pdFALSE);

    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri = "mqtt://164.41.98.25",
        .credentials.username = "0SQWDAh6P6ijtFwb3Bqa",
    };

    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

    // xSemaphoreGive(conexaoMQTTSemaphore);
}

void mqtt_envia_mensagem(char *topico, char *mensagem)
{
    int message_id = -1;
    if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
    {
        message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
        xSemaphoreGive(conexaoMQTTSemaphore);
    }

    if (message_id == -1)
    {
        ESP_LOGE(TAG_M, "Topico: %s\nFalha ao enviar a mensagem: %s\n", topico, mensagem);
    }
    else
    {
        ESP_LOGI(TAG_M, "ID da mensagem: %d\nMensagem enviada: %s", message_id, mensagem);
    }
}

void mqtt_disconnect()
{
    while (full_power_mode && !xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
        ;

    esp_mqtt_client_stop(client);
    esp_mqtt_client_unregister_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler);
    esp_mqtt_client_disconnect(client);
    esp_mqtt_client_destroy(client);
}

void mqtt_reconnect()
{
    esp_mqtt_client_reconnect(client);
}
