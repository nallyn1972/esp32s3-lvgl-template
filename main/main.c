/**
 * @file main.c
 * @brief ESP32-S3 LVGL Template - Main Application
 * 
 * This template provides a complete setup for ESP32-S3 with:
 * - ILI9341 320x240 LCD display via SPI
 * - LVGL GUI framework (v9.x)
 * - EC11 rotary encoder with button for input
 * - PWM backlight control
 * 
 * Based on the breadproofbox project hardware configuration.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_ili9341.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "ec11_encoder.h"

#include "hardware_config.h"

static const char *TAG = "LVGL_TEMPLATE";

// LCD and LVGL handles
static esp_lcd_panel_io_handle_t lcd_io = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;
static lv_display_t *lvgl_disp = NULL;
static lv_indev_t *lvgl_encoder_indev = NULL;
static lv_group_t *default_group = NULL;

// =============================================================================
// LCD Initialization
// =============================================================================

static esp_err_t lcd_init(void)
{
    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Initialize SPI bus");
    const spi_bus_config_t bus_config = {
        .sclk_io_num = LCD_PIN_NUM_SCLK,
        .mosi_io_num = LCD_PIN_NUM_MOSI,
        .miso_io_num = LCD_PIN_NUM_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * 50 * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &bus_config, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = LCD_PIN_NUM_LCD_DC,
        .cs_gpio_num = LCD_PIN_NUM_LCD_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &lcd_io));

    ESP_LOGI(TAG, "Install ILI9341 panel driver");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LCD_PIN_NUM_LCD_RST,
        .color_space = ESP_LCD_COLOR_SPACE_BGR,
        .bits_per_pixel = LCD_BITS_PER_PIXEL,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(lcd_io, &panel_config, &lcd_panel));

    ESP_LOGI(TAG, "Initialize LCD panel");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(lcd_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(lcd_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(lcd_panel, false, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(lcd_panel, true));

    ESP_LOGI(TAG, "LCD initialization complete");
    return ret;
}

// =============================================================================
// Backlight Initialization
// =============================================================================

static esp_err_t backlight_init(void)
{
    ESP_LOGI(TAG, "Initialize backlight (PWM)");
    
    ledc_timer_config_t ledc_timer = {
        .speed_mode = BK_LIGHT_MODE,
        .duty_resolution = BK_LIGHT_DUTY_RES,
        .timer_num = BK_LIGHT_TIMER,
        .freq_hz = BK_LIGHT_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode = BK_LIGHT_MODE,
        .channel = BK_LIGHT_CHANNEL,
        .timer_sel = BK_LIGHT_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = BK_LIGHT_OUTPUT_IO,
        .duty = BK_LIGHT_DUTY,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_LOGI(TAG, "Backlight initialized at %d%% brightness", 
             (int)((BK_LIGHT_DUTY * 100) / BK_LIGHT_MAX_DUTY));
    return ESP_OK;
}

// =============================================================================
// Encoder Initialization using EC11 Component
// =============================================================================

static esp_err_t encoder_init(void)
{
    ESP_LOGI(TAG, "Initialize EC11 encoder component");
    
    // Configure encoder
    ec11_encoder_config_t encoder_config = {
        .gpio_a = EC11_GPIO_A,
        .gpio_b = EC11_GPIO_B,
        .gpio_button = EC11_GPIO_BUTTON,
        .button_active_low = (BUTTON_ACTIVE_LEVEL == 0),
        .debounce_ms = 8
    };
    
    // Initialize encoder
    ESP_ERROR_CHECK(ec11_encoder_init(&encoder_config));
    
    ESP_LOGI(TAG, "EC11 encoder initialized successfully");
    return ESP_OK;
}

// =============================================================================
// LVGL Initialization
// =============================================================================

static esp_err_t lvgl_init(void)
{
    ESP_LOGI(TAG, "Initialize LVGL port");
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = LVGL_TASK_PRIORITY,
        .task_stack = LVGL_TASK_STACK_SIZE,
        .task_affinity = -1,
        .task_max_sleep_ms = LVGL_TASK_MAX_DELAY_MS,
        .timer_period_ms = LVGL_TICK_PERIOD_MS,
    };
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    ESP_LOGI(TAG, "Add LCD display");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = lcd_io,
        .panel_handle = lcd_panel,
        .buffer_size = (LCD_H_RES * LCD_V_RES) / 10,
        .double_buffer = false,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = false,
        .color_format = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy = false,
            .mirror_x = true,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true,
            .swap_bytes = true,
        }
    };
    lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    // Initialize encoder component
    ESP_ERROR_CHECK(encoder_init());

    // Create LVGL input device for encoder
    ESP_LOGI(TAG, "Create LVGL encoder input device");
    lvgl_encoder_indev = ec11_encoder_create_lvgl_indev();
    
    // Create default group and set as default
    default_group = lv_group_create();
    lv_group_set_default(default_group);
    lv_indev_set_group(lvgl_encoder_indev, default_group);
    
    // Configure group to reduce navigation sensitivity
    lv_group_set_wrap(default_group, true);  // Allow wrapping around items

    ESP_LOGI(TAG, "LVGL initialization complete");
    return ESP_OK;
}

// =============================================================================
// Example LVGL UI - Simple Demo Screen
// =============================================================================

// Slider value changed callback
static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
    int32_t value = lv_slider_get_value(slider);
    lv_label_set_text_fmt(label, "%d", (int)value);
}

static void create_demo_ui(void)
{
    // Lock LVGL mutex before creating UI
    lvgl_port_lock(0);

    // Create a simple label as example
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x003a57), LV_PART_MAIN);

    // Title label
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "ESP32-S3 LVGL Template");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // Instructions label
    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, 
        "Hardware Ready!\n\n"
        "- Rotate encoder to test\n"
        "- Press button to interact\n\n"
        "Modify main.c to\n"
        "create your app");
    lv_obj_set_style_text_color(info, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, 0);

    // Create first slider as interactive example
    lv_obj_t *slider1 = lv_slider_create(scr);
    lv_obj_set_width(slider1, 200);
    lv_obj_align(slider1, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_slider_set_range(slider1, 0, 100);
    lv_slider_set_value(slider1, 50, LV_ANIM_OFF);
    
    // Add first slider to group so encoder can control it
    lv_group_add_obj(default_group, slider1);
    lv_group_focus_obj(slider1);

    // Value label for first slider
    lv_obj_t *value_label1 = lv_label_create(scr);
    lv_label_set_text(value_label1, "50");
    lv_obj_set_style_text_color(value_label1, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align_to(value_label1, slider1, LV_ALIGN_OUT_TOP_MID, 0, -10);

    // Add event to update label when first slider changes
    lv_obj_add_event_cb(slider1, slider_event_cb, LV_EVENT_VALUE_CHANGED, value_label1);

    // Create second slider with different range
    lv_obj_t *slider2 = lv_slider_create(scr);
    lv_obj_set_width(slider2, 180);
    lv_obj_align(slider2, LV_ALIGN_BOTTOM_MID, 0, -15);
    lv_slider_set_range(slider2, -50, 50);
    lv_slider_set_value(slider2, 0, LV_ANIM_OFF);
    
    // Add second slider to group so encoder can control it
    lv_group_add_obj(default_group, slider2);

    // Value label for second slider  
    lv_obj_t *value_label2 = lv_label_create(scr);
    lv_label_set_text(value_label2, "0");
    lv_obj_set_style_text_color(value_label2, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align_to(value_label2, slider2, LV_ALIGN_OUT_TOP_MID, 0, -10);

    // Add event to update label when second slider changes
    lv_obj_add_event_cb(slider2, slider_event_cb, LV_EVENT_VALUE_CHANGED, value_label2);

    // Unlock LVGL mutex
    lvgl_port_unlock();

    ESP_LOGI(TAG, "Demo UI created");
}

// =============================================================================
// Main Application
// =============================================================================

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32-S3 LVGL Template Starting...");
    ESP_LOGI(TAG, "Hardware: ESP32-S3, ILI9341 LCD, EC11 Encoder");

    // Initialize hardware
    ESP_ERROR_CHECK(backlight_init());
    ESP_ERROR_CHECK(lcd_init());
    ESP_ERROR_CHECK(lvgl_init());

    // Create demo UI
    create_demo_ui();

    ESP_LOGI(TAG, "Template ready! Modify create_demo_ui() to build your application.");

    // Main loop - LVGL tasks run in background
    while (1) {
        // Your application code here
        // LVGL updates automatically via lvgl_port
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
