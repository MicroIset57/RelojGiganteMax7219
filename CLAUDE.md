# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Giant clock display for ISET 57 using ESP32 microcontroller. The clock displays time on 7-segment displays controlled by a MAX7219 chip and shows days of the week using WS2812B RGB LEDs.

Hardware components:
- ESP32 DevKit V1
- MAX7219 driving 4x 7-segment displays (PAROLA_HW type)
- WS2812B RGB LED strip (52 LEDs total: 35 for weekdays + 17 for "ISET 57")
- WiFi connectivity for NTP time synchronization

**Known Issue**: No RTC with battery backup - time is lost on power failure.

## Build & Development Commands

### Build and Upload
```bash
pio run -e esp32-reloj                    # Build the project
pio run -e esp32-reloj -t upload          # Upload to ESP32
pio run -e esp32-reloj -t monitor         # Open serial monitor (115200 baud)
pio run -e esp32-reloj -t upload -t monitor  # Upload and monitor
```

### Debug Configuration
The brownout detector is disabled in platformio.ini to prevent spurious resets during LED operations. To enable debug logs, uncomment these lines in platformio.ini:
```ini
-DCORE_DEBUG_LEVEL=5
-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE
```

Serial monitor includes `esp32_exception_decoder` filter for crash analysis.

## Hardware Configuration

### MAX7219 7-Segment Display
- **Type**: PAROLA_HW (MD_MAX72XX library)
- **SPI Pins**: CLK=18, DATA=23, CS=5
- **Displays**: 1 device controlling 4 digits + 1 decimal point digit
- **Segment mapping**: Custom for PAROLA_HW (see SEGA-SEGG defines)
- **Digit mapping**: DIG0-DIG3 for HH:MM, DIG4 for colon dots

### WS2812B RGB LEDs (FastLED)
- **Pin**: GPIO 15
- **Count**: 52 LEDs
- **Layout**:
  - Lunes (Monday): LEDs 44-50
  - Martes (Tuesday): LEDs 38-43
  - Miércoles (Wednesday): LEDs 31-36 (skip 37)
  - Jueves (Thursday): LEDs 24-30
  - Viernes (Friday): LEDs 18-23 (skip 17)
  - "ISET 57" text: LEDs 0-16 (skip 17)

### WiFi Configuration
WiFi credentials are in [src/config.h](src/config.h). Edit this file to change SSID/password.

## Code Architecture

### Core Components

**reloj-gigante.cpp** - Main application file
- `setup()`: Initializes MAX7219, WS2812B LEDs, runs hardware tests
- `loop()`: Manages WiFi connection, NTP sync, time display, LED animations
- `setNumero(digito, num)`: Renders a single digit (0-9) on 7-segment display
- `setHora(hr)`, `setMinuto(min)`: Display hour and minute
- `setPuntos()`: Toggles colon between hour and minute every 500ms
- `setDias(dia)`: Lights up weekday indicator (1=Mon through 5=Fri)

**Leds.cpp/h** - WS2812B LED control abstraction
- `LedsInit()`: Initialize FastLED library
- `AllLeds(color, ...)`: Set all or range of LEDs
- `updateRainbow()`: Animated rainbow effect (used on weekends)
- `NextLed(...)`: Moving LED effect (used on weekdays for "ISET 57")
- `LedsKitt(...)`: KITT car scanner effect (used in startup test)

**miWebServer.cpp/h** - WiFi and NTP time synchronization
- `wifiConnect()`: Connects to WiFi (30 second timeout, then calls `esp_restart()`)
- `syncTimeWithNTP()`: Synchronizes ESP32 time via NTP (runs every hour)
  - Uses 3 NTP servers: time.google.com, time.cloudflare.com, ar.pool.ntp.org
  - Timezone: UTC-3 (Argentina)
  - Includes 100ms delay in polling loop to prevent watchdog resets
- `isTimeSet()`: Validates time is set (checks if after Oct 9, 2025)

**config.h** - WiFi credentials (not tracked in git typically)

### Critical Design Details

**NTP Synchronization Caching**:
The `syncTimeWithNTP()` function caches successful sync timestamp and only re-syncs after 1 hour. The variable `lasTimeAtHourOk` must only be updated AFTER successful sync to prevent premature caching.

**Watchdog Timer Management**:
ESP32 has a default ~5 second watchdog. Long-running operations need periodic delays:
- NTP sync loop includes `delay(100)` every iteration
- Main loop includes `delay(10)` at end
- Avoid tight loops without delays

**7-Segment Display Updates**:
The `setNumero()` function does NOT call `mx.clear()` to avoid flickering and potential SPI issues. Each digit's segments are explicitly set to true/false state.

**Weekend Mode**:
- Saturday/Sunday (`tm_wday` 0 or 6): Shows rainbow animation on all LEDs
- Weekdays: Shows day indicator + random color animation on "ISET 57" text

**Decimal Point Usage**:
DIG4 is used for the colon separating hours and minutes. The SEGD segment (not SEGDP) is toggled due to hardware wiring.

## Debugging Reset Issues

If experiencing unexpected resets after time sync:
1. Check serial output shows "Hora sincronizada" message
2. Verify `lasTimeAtHourOk` is only updated after successful NTP response
3. Ensure all loops include delays (watchdog starvation)
4. Check brownout detector settings if power supply is marginal
5. Use `esp32_exception_decoder` monitor filter to decode crash backtraces

Boot loop (constant resets): Usually indicates stack overflow, memory corruption, or peripheral initialization conflict. Check global object constructors and setup() initialization order.
