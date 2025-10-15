/**
 * @file hardware_config.h
 * @brief Hardware pin definitions for ESP32-S3 LVGL Template
 * 
 * This file contains all hardware pin definitions and constants
 * for the ESP32-S3 board with ILI9341 LCD and EC11 rotary encoder.
 * 
 * Hardware Configuration:
 * - ESP32-S3 with 32MB Flash, 8MB PSRAM (Octal)
 * - ILI9341 320x240 LCD (SPI interface)
 * - EC11 Rotary Encoder with button
 * - PWM backlight control
 */

#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// LCD Display Configuration (ILI9341 via SPI)
// =============================================================================

// SPI Bus Configuration
#define LCD_HOST            SPI2_HOST
#define LCD_PIN_NUM_SCLK    12
#define LCD_PIN_NUM_MOSI    11
#define LCD_PIN_NUM_MISO    13

// LCD Control Pins
#define LCD_PIN_NUM_LCD_DC  10
#define LCD_PIN_NUM_LCD_CS  9
#define LCD_PIN_NUM_LCD_RST 14

// LCD Display Parameters
#define LCD_H_RES           320
#define LCD_V_RES           240
#define LCD_PIXEL_CLOCK_HZ  (40 * 1000 * 1000)
#define LCD_CMD_BITS        8
#define LCD_PARAM_BITS      8
#define LCD_BITS_PER_PIXEL  16

// =============================================================================
// Backlight Configuration (PWM/LEDC)
// =============================================================================

#define BK_LIGHT_OUTPUT_IO  8
#define BK_LIGHT_MODE       LEDC_LOW_SPEED_MODE
#define BK_LIGHT_CHANNEL    LEDC_CHANNEL_0
#define BK_LIGHT_TIMER      LEDC_TIMER_0
#define BK_LIGHT_DUTY_RES   LEDC_TIMER_13_BIT
#define BK_LIGHT_FREQ_HZ    5000
#define BK_LIGHT_MAX_DUTY   ((1 << 13) - 1)  // 8191 for 13-bit resolution
#define BK_LIGHT_DUTY       (BK_LIGHT_MAX_DUTY * 70 / 100)  // 70% default brightness

// =============================================================================
// Rotary Encoder Configuration (EC11)
// =============================================================================

#define EC11_GPIO_A         1   // Encoder phase A
#define EC11_GPIO_B         2   // Encoder phase B
#define EC11_GPIO_BUTTON    3   // Encoder button (push)

// Button configuration
#define BUTTON_ACTIVE_LEVEL 0   // Active low

// =============================================================================
// LVGL Configuration Constants
// =============================================================================

#define LVGL_TASK_PRIORITY       5
#define LVGL_TASK_STACK_SIZE     (6 * 1024)
#define LVGL_TASK_MAX_DELAY_MS   500
#define LVGL_TICK_PERIOD_MS      5

#ifdef __cplusplus
}
#endif

#endif // HARDWARE_CONFIG_H
