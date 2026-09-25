## Circuit Diagram & Wiring Guide

### 1. Fingerprint Sensor (AS608 / R307)
| Sensor Wire | ESP32 Pin     | Notes                  |
|-------------|---------------|------------------------|
| Red         | 5V (VIN)      | Power                  |
| Black       | GND           | Ground                 |
| White       | GPIO 16       | Sensor TX → ESP32 RX   |
| Yellow      | GPIO 17       | Sensor RX → ESP32 TX   |
| Blue        | Not connected | Optional (Touch)       |
| Extra wire  | Not connected | Optional               |

### 2. LEDs
| LED         | ESP32 Pin | Connection                          |
|-------------|-----------|-------------------------------------|
| Red LED     | GPIO 26   | Anode → 220Ω resistor → GPIO 26     |
|             |           | Cathode → GND                       |
| Green LED   | GPIO 27   | Anode → 220Ω resistor → GPIO 27     |
|             |           | Cathode → GND                       |

### 3. SD Card Module
| SD Module Pin | ESP32 Pin |
|---------------|-----------|
| VCC           | 3.3V      |
| GND           | GND       |
| CS            | GPIO 5    |
| MOSI          | GPIO 23   |
| MISO          | GPIO 19   |
| SCK           | GPIO 18   |

> **Important**: Always power the SD card module with **3.3V**, never 5V.

### 4. Power Supply (Optional Battery Setup)
- Battery (3.7V LiPo) → TP4056 B+ / B-
- TP4056 OUT+ → ESP32 VIN
- TP4056 OUT- → ESP32 GND
- Optional push button between TP4056 OUT+ and ESP32 VIN for power on/off

### Connection Notes
- Make sure all grounds (GND) are connected together.
- Use a good quality USB cable or stable 5V power supply.
- Keep fingerprint sensor wires as short as possible for better communication.