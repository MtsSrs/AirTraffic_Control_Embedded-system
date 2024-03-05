#include "mqtt.h"

static esp_mqtt_client_handle_t client;

void handle_response(char *data)
{
    ESP_LOGW(TAG_M, "Chegou aqui\n%s\n", data);

    cJSON *root = cJSON_Parse(data);

    char *response = cJSON_Print(root);

    cJSON *method = cJSON_GetObjectItem(root, "checkStatus");

    ESP_LOGW(TAG_M, "DATA %s\n", response);
    ESP_LOGW(TAG_M, "LED TESTE: %s", method->valuestring);

    // cJSON *method = cJSON_GetObjectItem(root, "method");
    // cJSON *params = cJSON_GetObjectItem(root, "params");
    // cJSON *objectParams = cJSON_GetObjectItem(params, "button");

    // char command[10];
    // strcpy(command, objectParams->valuestring);

    // if (!strcmp(command, "1"))
    //     ESP_LOGW(TAG_M, "LED ATIVADO: %s", objectParams->valuestring);
    // else
    //     ESP_LOGW(TAG_M, "LED DESATIVADO: %s", objectParams->valuestring);

    // if (!strcmp(command, "reset"))
    // {
    //     gpio_set_level(LED, LOW);
    //     gpio_set_level(BUZZER, LOW);
    //     esp_restart();
    // }
    // else if (!strcmp(method->valuestring, "setLowPower"))
    // {
    //     cJSON *tes = cJSON_GetObjectItem(method, "params");
    //     printf("%s", tes->valuestring);

    //     if (tes->valueint)
    //     {
    //         grava_valor_nvs(1);
    //         esp_restart();
    //     }
    //     else
    //     {
    //         grava_valor_nvs(0);
    //         esp_restart();
    //     }
    // }

    /* ESP_LOGW(TAG_M, "METHOD GOT: %s", method->valuestring);
    ESP_LOGW(TAG_M, "PARAMS GOT: %d", objectParams->valueint); */
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
        .credentials.username = "UusiSsj1CsoeOb5k8t58",
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
