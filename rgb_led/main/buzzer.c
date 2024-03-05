#include "esp_sleep.h"
#include "esp_system.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"

#include "buzzer.h"

void init_buzzer()
{
    esp_rom_gpio_pad_select_gpio(BUZZER);
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
}

void run_buzzer()
{
    gpio_set_level(BUZZER, true);
}

void stop_buzzer()
{
    gpio_set_level(BUZZER, false);
}
