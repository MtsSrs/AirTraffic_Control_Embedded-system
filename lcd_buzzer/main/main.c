#include <inttypes.h>
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "wifi.h"
#include "buzzer.h"
#include "lcd.h"
#include "globals.h"

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();

    wifi_start();
    mqtt_start();
    init_buzzer();

    while (true)
    {
        // show_lcd_data();
        run_buzzer();
        vTaskDelay(pdMS_TO_TICKS(4000));
        stop_buzzer();
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}
