# PingPal Firmware 🚀

PingPal is a standalone ESP32-based network monitoring device that continuously pings a target host
and displays online/offline status, latency, and time-since-last-ping on an OLED display.

This repository contains **firmware only** (no cloud, no user accounts, no backend).
It is designed to be **stable, local-first, and hackable**.

---

## ✨ Features

- ESP32-based (dual-core, FreeRTOS)
- Non-blocking ping logic (no UI freeze)
- OLED status display
- Physical button for setup/reset flows
- Wi-Fi captive portal for first-time setup
- LED status indicator
- Watchdog-protected main loop
- Designed for long uptime

---

## 🧠 How It Works (High Level)

- **Core 1 (Arduino loop task)**  
  Handles:
  - UI updates
  - State machine
  - Button input
  - Wi-Fi events

- **Ping Service**  
  Runs ping logic without blocking the main loop.
  Even if ping stalls or Wi-Fi glitches, UI stays responsive.

---

## 🧩 Hardware Requirements

| Component | Notes |
|---------|------|
| ESP32 Dev Module | Tested with ESP32-WROOM |
| OLED Display | SSD1306 (128x64) |
| Push Button | Momentary |
| RGB LED | Common cathode |
| Resistors | 220Ω for LED |
| Breadboard / PCB | Optional |

---

## 🔌 Pin Configuration (Default)

These are **defaults** used in firmware.
You can change them in code if needed.

### Button
```cpp
Button button(5);
```
- **GPIO 5** → Button signal
- Other leg → GND

### RGB LED
```cpp
Led led(23, 19, 18);
```
- **GPIO 23** → Red
- **GPIO 19** → Green
- **GPIO 18** → Blue
- Common cathode → GND (via resistor)

### OLED Display (I2C)
Most SSD1306 modules use default ESP32 I2C pins:

- **SDA** → GPIO 21
- **SCL** → GPIO 22
- **VCC** → 3.3V
- **GND** → GND

If your OLED uses different pins, update the OLED driver init.

---

## 🛠️ Changing Hardware Pins

1. Open `PingPalApp.cpp`
2. Update constructor values:

```cpp
PingPalApp::PingPalApp()
    : button(NEW_GPIO),
      led(R, G, B)
``

3. Rebuild & flash

---

## 📦 Project Structure

```
.
├── src/
│   ├── PingPalApp.cpp / .h   # Main app + state machine glue
│   ├── PingService.cpp / .h  # Ping logic
│   ├── StateMachine.*        # State handling
│   └── hardware/
│       ├── Button.*
│       ├── Led.*
│       └── OledDisplay.*
├── platformio.ini
└── README.md
```

---

## ⚙️ Build & Flash

### Prerequisites

- VS Code
- PlatformIO extension
- ESP32 USB drivers

### Clone

```bash
git clone https://github.com/frizzycodes/ping-pal-firmware.git
cd ping-pal-firmware
```

### Build

```bash
pio run
```

### Flash

```bash
pio run -t upload
```

### Monitor (optional)

```bash
pio device monitor
```

---

## 🌐 Wi-Fi Setup

1. Power on device
2. If no Wi-Fi credentials found:
   - Device starts AP: `PingPal-Setup`
3. Connect via phone/laptop
4. Open browser → `192.168.4.1`
5. Select Wi-Fi + password
6. Device reboots and connects

---

## 🎯 Ping Configuration

Defaults (in `PingService.cpp`):

```cpp
targetHost = "8.8.8.8";
intervalMs = 10000; // 10 seconds
```

You can change:
- Target host (e.g. router, DNS, server)
- Ping interval

Rebuild after changes.

---

## 🔐 Watchdog Safety

The firmware uses ESP32 **Task Watchdog**:
- Protects against deadlocks
- Resets device if loop stalls
- Safe for long-running uptime

---

## 🧪 Stability Notes

- Timer drift of ±1s is **normal**
- Ping blocking is guarded
- Wi-Fi disconnects handled gracefully
- No memory leaks observed in long runs

---

## 🚦 Versioning

- **v1.0.0** → Stable local-only firmware
- Future versions may add:
  - Server reporting
  - User accounts
  - Web dashboard

---

## 🧑‍💻 Customization Ideas

- Change ping host dynamically
- Adjust ping interval
- Add buzzer for offline alerts
- Add SD card logging
- Expose REST API

---

## 📜 License

MIT License — hack it, ship it, improve it.
