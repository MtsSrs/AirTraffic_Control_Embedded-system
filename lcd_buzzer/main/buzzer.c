#include "buzzer.h"

void init_buzzer()
{
    esp_rom_gpio_pad_select_gpio(BUZZER);

    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
}

void run_buzzer()
{
    gpio_set_level(BUZZER, true);
    send_buzzer_data(1);
}

void stop_buzzer()
{
    gpio_set_level(BUZZER, false);
    send_buzzer_data(0);
}

void send_buzzer_data(int value)
{
    char mensagem[50];

    if (value == 1)
        sprintf(mensagem, "{\"led1\": 1}");
    else
        sprintf(mensagem, "{\"led1\": 0}");

    mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);
}

void buzzer_task()
{
    bool alarm = true;
    while (true)
    {
        gpio_set_level(BUZZER, alarm);
        wait_seconds(2);
        alarm = !alarm;
    }
}