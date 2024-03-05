#ifndef LCD_H
#define LCD_H

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include <esp_lcd_types.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_dev.h>

#include "globals.h"

#define I2C_MASTER_NUM 0
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_TIMEOUT_MS 1000

#define MPU9250_SENSOR_ADDR 0x68
#define MPU9250_WHO_AM_I_REG_ADDR 0x75

#define MPU9250_PWR_MGMT_1_REG_ADDR 0x6B
#define MPU9250_RESET_BIT 7

static const char *TAG = "i2c-simple-example";

void show_lcd_data();

#endif