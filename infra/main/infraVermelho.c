#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "mqtt.h"


#define SENSOR_PROXIMIDADE  15 // deixei a protoboard com esse
   
void infra() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SENSOR_PROXIMIDADE),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&io_conf);

    printf("Entre na função\n");

    while (1) {
        int estadoSensor = gpio_get_level(SENSOR_PROXIMIDADE);

        if (estadoSensor == 0) {
            printf("Objeto detectado\n");
                // enviando para a esp a presença de um objeto pra acender a led
                 mqtt_envia_mensagem("v1/devices/me/attributes", "{proximitySensor: 1}");
        } else {
            printf("Nenhum objeto detectado\n");
            mqtt_envia_mensagem("v1/devices/me/attributes", "{proximitySensor: 0}");
        }
        
        // pra n pegar repetido o movimento
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
