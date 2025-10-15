# ESP32-S3 LVGL Template - Setup Guide

This guide covers setting up the development environment on both **Linux** and **Windows 11**.

## Prerequisites

### Both Platforms
- Visual Studio Code
- Git
- USB drivers for ESP32 (usually automatic on Linux, may need drivers on Windows)

### Platform-Specific

#### Linux
- Python 3.8 or newer
- Build essentials: `sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0`

#### Windows 11
- Python 3.8 or newer (from [python.org](https://www.python.org/downloads/))
- Git for Windows
- PowerShell or Command Prompt

---

## Installation

### 1. Install ESP-IDF

#### Linux

```bash
# Create esp directory
mkdir -p ~/esp
cd ~/esp

# Clone ESP-IDF v5.3
git clone -b v5.3.1 --recursive https://github.com/espressif/esp-idf.git

# Install ESP-IDF
cd ~/esp/esp-idf
./install.sh esp32

# Setup environment (add to ~/.bashrc for persistence)
. ~/esp/esp-idf/export.sh
```

To make ESP-IDF available in every terminal, add to `~/.bashrc`:
```bash
alias get_idf='. $HOME/esp/esp-idf/export.sh'
```

#### Windows 11

**Option 1: ESP-IDF Installer (Recommended)**
1. Download the [ESP-IDF Windows Installer](https://dl.espressif.com/dl/esp-idf/)
2. Run the installer and follow the wizard
3. Select "ESP32" as the target
4. Install with default options

**Option 2: Manual Installation**
```powershell
# Open PowerShell and create esp directory
mkdir $HOME\esp
cd $HOME\esp

# Clone ESP-IDF v5.3
git clone -b v5.3.1 --recursive https://github.com/espressif/esp-idf.git

# Install ESP-IDF
cd esp-idf
.\install.ps1 esp32

# Setup environment
.\export.ps1
```

---

### 2. Install VS Code Extensions

Install these extensions in VS Code:

1. **Espressif IDF** (`espressif.esp-idf-extension`)
   - Official ESP-IDF extension with build, flash, and debug support
   
2. **C/C++** (`ms-vscode.cpptools`)
   - IntelliSense, debugging, and code browsing
   
3. **CMake Tools** (`ms-vscode.cmake-tools`) - Optional but recommended

---

### 3. Clone and Setup Project

#### Linux
```bash
cd ~/workspace  # or your preferred directory
git clone https://github.com/nallyn1972/esp32s3-lvgl-template.git
cd esp32s3-lvgl-template

# Activate ESP-IDF environment
. ~/esp/esp-idf/export.sh

# Build the project
idf.py build
```

#### Windows
```powershell
cd $HOME\workspace  # or your preferred directory
git clone https://github.com/nallyn1972/esp32s3-lvgl-template.git
cd esp32s3-lvgl-template

# Activate ESP-IDF environment (if using manual installation)
$HOME\esp\esp-idf\export.ps1

# Build the project
idf.py build
```

---

### 4. Configure VS Code

#### Open Project in VS Code
```bash
# Linux
code ~/workspace/esp32s3-lvgl-template

# Windows
code $HOME\workspace\esp32s3-lvgl-template
```

#### Configure ESP-IDF Extension

1. Press `Ctrl+Shift+P` (or `Cmd+Shift+P` on Mac)
2. Type `ESP-IDF: Configure ESP-IDF Extension`
3. Select your ESP-IDF installation:
   - **Linux**: `~/esp/esp-idf`
   - **Windows**: `C:\Users\<username>\esp\esp-idf` or where you installed it
4. Select Python path (the extension usually finds it automatically)
5. Select the tools path:
   - **Linux**: `~/.espressif`
   - **Windows**: `C:\Users\<username>\.espressif`

#### Configure Serial Port

Update `.vscode/settings.json` with your serial port:

**Linux:**
```jsonc
{
  "idf.port": "/dev/ttyUSB0"  // or /dev/ttyACM0, check with: ls /dev/tty*
}
```

**Windows:**
```jsonc
{
  "idf.portWin": "COM3"  // Check Device Manager for correct COM port
}
```

---

## Building and Flashing

### Using VS Code Tasks

Press `Ctrl+Shift+B` to build, or:
- `Ctrl+Shift+P` → `Tasks: Run Task`
  - **Build - ESP-IDF**: Compile the project
  - **Clean - ESP-IDF**: Clean build files
  - **Flash - ESP-IDF**: Flash to device
  - **Monitor - ESP-IDF**: Open serial monitor
  - **Flash & Monitor - ESP-IDF**: Flash and monitor

### Using Command Line

#### Linux
```bash
# Make sure ESP-IDF is in your environment
. ~/esp/esp-idf/export.sh

# Build
idf.py build

# Flash (connect ESP32 via USB)
idf.py -p /dev/ttyUSB0 flash

# Monitor serial output
idf.py -p /dev/ttyUSB0 monitor

# Flash and monitor
idf.py -p /dev/ttyUSB0 flash monitor
```

#### Windows
```powershell
# Make sure ESP-IDF is in your environment
$HOME\esp\esp-idf\export.ps1

# Build
idf.py build

# Flash (connect ESP32 via USB)
idf.py -p COM3 flash

# Monitor serial output
idf.py -p COM3 monitor

# Flash and monitor
idf.py -p COM3 flash monitor
```

---

## Troubleshooting

### IntelliSense Not Working

1. Build the project first: `idf.py build`
2. Make sure `build/compile_commands.json` exists
3. In VS Code: `Ctrl+Shift+P` → `C/C++: Reset IntelliSense Database`
4. Select the correct configuration:
   - Linux: "ESP-IDF (Linux)"
   - Windows: "ESP-IDF (Windows)"

### Permission Denied on Serial Port (Linux)

```bash
# Add your user to dialout group
sudo usermod -a -G dialout $USER

# Log out and log back in, or run:
newgrp dialout
```

### Cannot Find COM Port (Windows)

1. Open Device Manager
2. Look under "Ports (COM & LPT)"
3. Find "Silicon Labs CP210x" or "CH340" device
4. Note the COM port number (e.g., COM3)
5. Install drivers if needed:
   - [CP210x drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
   - [CH340 drivers](https://sparks.gogo.co.nz/ch340.html)

### Build Fails with "Tool doesn't match supported version"

```bash
# Linux
cd ~/workspace/esp32s3-lvgl-template
rm -rf build
idf.py build

# Windows
cd $HOME\workspace\esp32s3-lvgl-template
Remove-Item -Recurse -Force build
idf.py build
```

### ESP-IDF Not in PATH

**Linux:** Make sure you run `. ~/esp/esp-idf/export.sh` in your terminal

**Windows:** Make sure you run `$HOME\esp\esp-idf\export.ps1` in PowerShell

---

## Project Structure

```
esp32s3-lvgl-template/
├── main/
│   ├── main.c              # Main application code
│   ├── hardware_config.h   # Hardware pin definitions
│   ├── idf_component.yml   # Component dependencies
│   └── CMakeLists.txt      # Component build configuration
├── .vscode/
│   ├── c_cpp_properties.json  # IntelliSense configuration
│   ├── settings.json          # VS Code settings
│   └── tasks.json             # Build tasks
├── CMakeLists.txt          # Main build configuration
├── README.md               # Project documentation
├── SETUP.md               # This file
└── .gitignore             # Git ignore patterns
```

---

## Next Steps

1. Modify `main/hardware_config.h` to match your hardware pinout
2. Edit `main/main.c` to create your application
3. Build and flash: `idf.py -p <PORT> flash monitor`
4. Start coding your LVGL UI!

---

## Resources

- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [LVGL Documentation](https://docs.lvgl.io/)
- [ESP32-S3 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
- [ILI9341 Display Driver](https://components.espressif.com/components/espressif/esp_lcd_ili9341)
- [Button Component](https://components.espressif.com/components/espressif/button)
- [Knob Component](https://components.espressif.com/components/espressif/knob)

---

## Support

If you encounter issues:
1. Check the [ESP-IDF GitHub Issues](https://github.com/espressif/esp-idf/issues)
2. Visit the [ESP32 Forum](https://esp32.com/)
3. Open an issue in this repository
