# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an ESP32-based giant clock display for ISET 57. It uses MAX7219 LED drivers to control four 7-segment displays for time, and WS2812B RGB LEDs to display days of the week and the ISET 57 logo. The clock synchronizes time via NTP over WiFi.

## Hardware Architecture

- **Display Controller**: MAX7219 controlling 4 seven-segment displays (hours and minutes)
- **Day Indicators**: WS2812B RGB LEDs (52 total):
  - Lunes (Monday): LEDs 44-50
  - Martes (Tuesday): LEDs 38-43
  - Miércoles (Wednesday): LEDs 31-36 (skip 37)
  - Jueves (Thursday): LEDs 24-30
  - Viernes (Friday): LEDs 18-23 (skip 17)
  - ISET 57 logo: LEDs 0-16
- **Microcontroller**: ESP32 DevKit v1
- **Pin Configuration**:
  - MAX7219: CLK=18, DATA=23, CS=5
  - WS2812B: DATA=15

## Build and Upload Commands

```bash
# Build the project
pio run

# Upload to ESP32
pio run --target upload

# Monitor serial output (115200 baud)
pio device monitor

# Build and upload in one command
pio run --target upload && pio device monitor

# Clean build
pio run --target clean
```

## Code Architecture

### Main Components

1. **[reloj-gigante.cpp](src/reloj-gigante.cpp)**: Main program logic
   - Handles time display using 7-segment displays via MAX7219
   - Manages LED animations for weekdays and weekend modes
   - Main loop handles WiFi reconnection, NTP sync, and display updates
   - Custom 7-segment digit rendering (0-9) using segment mapping

2. **[Leds.cpp](src/Leds.cpp) / [Leds.h](src/Leds.h)**: WS2812B LED control
   - FastLED-based LED effects (rainbow, kitt, shift, etc.)
   - 52 LEDs total: weekday indicators + ISET 57 logo
   - Functions: `LedsInit()`, `AllLeds()`, `updateRainbow()`, `LedsKitt()`, `NextLed()`

3. **[miWebServer.cpp](src/miWebServer.cpp) / [miWebServer.h](src/miWebServer.h)**: Network and time sync
   - WiFi multi-connection support (auto-connects to available networks)
   - NTP time synchronization (syncs every hour)
   - Web server endpoints for manual time setting (currently disabled in code)
   - Functions: `wifiConnect()`, `syncTimeWithNTP()`, `isTimeSet()`

4. **[config.h](src/config.h)**: Configuration constants
   - WiFi credentials (AP and station mode)
   - NTP server settings
   - Time zone offset (Argentina UTC-3)
   - WiFi/NTP sync intervals

### Display Segment Mapping (MAX7219)

The 7-segment displays use the following digit mapping (DIG0=hours tens, DIG1=hours ones, DIG2=minutes tens, DIG3=minutes ones, DIG4=colon dots):

```
Segments: A(top), B(top-right), C(bottom-right), D(bottom), E(bottom-left), F(top-left), G(middle)
```

The `setNumero()` function in [reloj-gigante.cpp:65-129](src/reloj-gigante.cpp#L65-L129) implements custom digit rendering by selectively turning segments on/off.

### Time Flow

1. `setup()`: Initialize MAX7219, WS2812B LEDs, WiFi, and perform startup animation
2. `loop()`:
   - Check WiFi connection (reconnect if needed)
   - Sync time with NTP (first time + hourly)
   - Update colon blink every 500ms
   - Update time display every minute
   - Run LED animations (rainbow on weekends, random color on weekdays)

### LED Behavior

- **Weekdays (Mon-Fri)**: Corresponding day LEDs light up in cyan, ISET 57 logo shows random color animation
- **Weekends (Sat-Sun)**: All LEDs display moving rainbow effect
- **No WiFi**: Red flash
- **NTP Sync Failed**: Yellow flash

## Dependencies

The project uses PlatformIO with the following libraries:
- **MD_MAX72xx**: Custom library in `src/MD_MAX72xx/` for MAX7219 LED driver control
- **FastLED**: RGB LED control library (WS2812B) in `lib/FastLED/`

These are vendored libraries (included in the repo), no external lib_deps in platformio.ini.

## Known Issues

- **No RTC**: If power is lost, the clock loses time and must resync via WiFi/NTP
- **Brownout detector disabled**: `CONFIG_ESP_SYSTEM_BROWNOUT_DET=0` in build flags to prevent brownout resets

## WiFi Configuration

Current networks configured in [miWebServer.cpp:122-124](src/miWebServer.cpp#L122-L124):
```cpp
wifiMulti.addAP("iPhone de Javi", "Efficast1");
wifiMulti.addAP("ISET57CLARO", "GONZALO1981");
wifiMulti.addAP("ISET57", "12345678");
```

To add new networks, modify these lines in `initWebServer()`.

## Time Zone

Configured for Argentina (UTC-3, no daylight saving) in [config.h:16](src/config.h#L16).
