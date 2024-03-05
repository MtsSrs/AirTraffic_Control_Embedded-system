#include <stdio.h>
#include <inttypes.h>
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "wifi.h"
#include "dht11.h"
#include "mqtt.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bmx280.h"
#include "my_nvs.h"
extern int powerSaving;

void init_bmx280(i2c_port_t i2c_port, bmx280_t **bmx280);
void read_bmx280(bmx280_t *bmx280);

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    bmx280_t *bmx280_sensor;
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

    DHT11_init(DHT11_PIN);
    init_bmx280(I2C_NUM_0, &bmx280_sensor);

    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
    {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    while (true)
    {
        if (powerSaving == 1) {
            printf("Modo: Econômico. Desativando Wi-Fi...\n");

            nvs_write(powerSaving, "powerSaving");
        
            wifi_stop();

           
            esp_sleep_enable_timer_wakeup(10 * 1000000); 
            esp_light_sleep_start();

           
            wifi_start();

           
            powerSaving = nvs_read("powerSaving");
            if (powerSaving == 0) {
                
                continue;
            }

            printf("Reativando Wi-Fi...\n");
           
            dht_data();
            read_bmx280(bmx280_sensor);
            vTaskDelay(pdMS_TO_TICKS(20000)); 
        }
        else if (powerSaving == 0)
        {
            printf("Modo: Normal.\n");

            dht_data();
            read_bmx280(bmx280_sensor);

            vTaskDelay(pdMS_TO_TICKS(4000));
        }
    }

    esp_restart();
}

void init_bmx280(i2c_port_t i2c_port, bmx280_t **bmx280)
{
    i2c_config_t i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = false,
        .scl_pullup_en = false,
        .master.clk_speed = 100000};

    ESP_ERROR_CHECK(i2c_param_config(i2c_port, &i2c_cfg));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_port, I2C_MODE_MASTER, 0, 0, 0));

    *bmx280 = bmx280_create(i2c_port);
    if (!(*bmx280))
    {
        ESP_LOGE("BMX280", "Could not create bmx280 driver.");
        return;
    }

    ESP_ERROR_CHECK(bmx280_init(*bmx280));

    bmx280_config_t bmx_cfg = BMX280_DEFAULT_CONFIG;
    ESP_ERROR_CHECK(bmx280_configure(*bmx280, &bmx_cfg));
}

void read_bmx280(bmx280_t *bmx280)
{
    ESP_ERROR_CHECK(bmx280_setMode(bmx280, BMX280_MODE_FORCE));
    do
    {
        vTaskDelay(pdMS_TO_TICKS(1));
    } while (bmx280_isSampling(bmx280));

    float temp = 0, pres = 0, hum = 0;
    ESP_ERROR_CHECK(bmx280_readoutFloat(bmx280, &temp, &pres, &hum));
    pres /= 100;
    ESP_LOGI("BMX280", "Pres: %f", pres);
    char mensagem[50];
    sprintf(mensagem, "{\"pres\" :%f}", pres);
    mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);
}