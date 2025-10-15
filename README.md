# ESP32-S3 LVGL Template

A ready-to-use ESP-IDF project template for ESP32-S3 with LVGL GUI framework, ILI9341 LCD display, and rotary encoder input.

**Cross-Platform**: Works on both **Linux** and **Windows 11** with VS Code.

## Hardware Configuration

This template is designed for the following hardware setup:

### ESP32-S3 Board
- **MCU**: ESP32-S3
- **Flash**: 32MB
- **PSRAM**: 8MB (Octal)

### Display
- **LCD**: ILI9341 320x240 pixels
- **Interface**: SPI
- **Pins**:
  - SCLK: GPIO 12
  - MOSI: GPIO 11
  - MISO: GPIO 13
  - DC: GPIO 10
  - CS: GPIO 9
  - RST: GPIO 14
  - Backlight: GPIO 8 (PWM controlled)

### Input Device
- **Encoder**: EC11 Rotary Encoder with button
- **Pins**:
  - Phase A: GPIO 1
  - Phase B: GPIO 2
  - Button: GPIO 3 (active low)

## Features

- ✅ Complete LCD initialization with ILI9341 driver
- ✅ LVGL 9.x integration via ESP-LVGL-Port
- ✅ Rotary encoder input device with button support
- ✅ PWM backlight control (default 70% brightness)
- ✅ Hardware pin definitions in separate header file
- ✅ Example demo UI with interactive slider
- ✅ FreeRTOS task management
- ✅ Clean code structure ready for your application

## Getting Started

📖 **For detailed setup instructions for both Linux and Windows, see [SETUP.md](SETUP.md)**

### Quick Start

#### Prerequisites

- ESP-IDF v5.3 installed ([Installation Guide](SETUP.md#installation))
- Visual Studio Code with ESP-IDF extension
- Python 3.8+
- ESP32-S3 board with the hardware configuration above

#### Build and Flash

**Linux:**
```bash
# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Navigate to project
cd esp32s3-lvgl-template

# Build
idf.py build

# Flash to ESP32-S3
idf.py -p /dev/ttyUSB0 flash monitor
```

**Windows:**
```powershell
# Set up ESP-IDF environment
$HOME\esp\esp-idf\export.ps1

# Navigate to project
cd esp32s3-lvgl-template

# Build
idf.py build

# Flash to ESP32-S3
idf.py -p COM3 flash monitor
```

#### Using VS Code

1. Open project in VS Code
2. Press `Ctrl+Shift+B` to build
3. Use Command Palette (`Ctrl+Shift+P`) → `Tasks: Run Task` → `Flash & Monitor - ESP-IDF`

## Project Structure

```
esp32s3-lvgl-template/
├── main/
│   ├── main.c              # Main application code
│   ├── hardware_config.h   # Hardware pin definitions
│   ├── CMakeLists.txt      # Component build config
│   └── idf_component.yml   # Component dependencies
├── .vscode/
│   ├── c_cpp_properties.json  # IntelliSense (Linux & Windows)
│   ├── settings.json          # VS Code settings
│   └── tasks.json             # Build/Flash tasks
├── CMakeLists.txt          # Project build config
├── README.md               # This file
└── SETUP.md                # Detailed setup guide
```

## Customizing the Template

### Adding Your UI

The demo UI is created in the `create_demo_ui()` function in `main.c`. Replace this function with your own UI code:

```c
static void create_demo_ui(void)
{
    lvgl_port_lock(0);
    
    // Your LVGL UI code here
    lv_obj_t *scr = lv_scr_act();
    // ... create your widgets ...
    
    lvgl_port_unlock();
}
```

### Modifying Hardware Pins

All hardware pin definitions are in `main/hardware_config.h`. Change the `#define` values to match your hardware:

```c
#define LCD_PIN_NUM_SCLK    12  // Change to your SCLK pin
#define LCD_PIN_NUM_MOSI    11  // Change to your MOSI pin
// ... etc
```

### Adjusting Backlight Brightness

Change the default brightness in `hardware_config.h`:

```c
#define BK_LIGHT_DUTY  (BK_LIGHT_MAX_DUTY * 70 / 100)  // 70% brightness
```

Or control it dynamically in your code:

```c
uint32_t duty = (BK_LIGHT_MAX_DUTY * desired_percent) / 100;
ledc_set_duty(BK_LIGHT_MODE, BK_LIGHT_CHANNEL, duty);
ledc_update_duty(BK_LIGHT_MODE, BK_LIGHT_CHANNEL);
```

## Dependencies

This project uses the following ESP-IDF components via the component registry:

- `espressif/esp_lcd_ili9341` - ILI9341 LCD driver
- `espressif/esp_lvgl_port` - LVGL port for ESP-IDF
- `lvgl/lvgl` - LVGL graphics library (v9.x)
- `espressif/knob` - Rotary encoder driver
- `espressif/button` - Button driver

Dependencies are automatically downloaded during the first build.

## Encoder Usage

The rotary encoder is configured as an LVGL input device:

- **Rotate**: Navigate between widgets / change values
- **Press**: Select / activate widget

Add widgets to the input group to make them encoder-controllable:

```c
lv_group_add_obj(default_group, your_widget);
```

## Troubleshooting

### Display shows nothing
- Check power connections
- Verify SPI pin connections match `hardware_config.h`
- Ensure backlight is on (GPIO 8)

### Encoder not responding
- Verify encoder pins match `hardware_config.h`
- Check encoder wiring and pull-up resistors
- Monitor serial output for errors

### Build errors
- Ensure ESP-IDF v5.0+ is installed
- Run `idf.py fullclean` and rebuild
- Check that all dependencies downloaded correctly

## Based On

This template is based on the [breadproofbox](https://github.com/nallyn1972/breadproofbox) project hardware configuration, providing a clean starting point for ESP32-S3 + LVGL projects.

## License

This template is provided as-is for any use.

## Resources

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [LVGL Documentation](https://docs.lvgl.io/)
- [ESP-LVGL-Port Documentation](https://github.com/espressif/esp-bsp/tree/master/components/esp_lvgl_port)
