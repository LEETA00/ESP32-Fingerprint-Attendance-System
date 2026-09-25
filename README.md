# ESP32-Fingerprint-Attendance-System

A complete biometric attendance system built with ESP32, fingerprint sensor, dual LEDs, SD card logging, and real-time Google Sheets integration.

## Features
- Fingerprint enrollment and identification
- Green LED = Attendance taken successfully
- Red LED = No match / Already signed in
- Local logging on SD card (`attendance.csv` & `users.csv`)
- Automatic upload to Google Sheets
- Prevents multiple sign-ins on the same day
- Serial Monitor commands for enrollment

## Hardware Required
- ESP32 Dev Module (WROOM)
- AS608 / R307 Fingerprint Sensor
- Micro SD Card Module
- Red LED + 220Ω resistor
- Green LED + 220Ω resistor
- Breadboard + Jumper wires
- Optional: Battery + TP4056 charging module

## Pin Configuration

| Component              | ESP32 Pin |
|------------------------|-----------|
| Fingerprint TX         | GPIO 16   |
| Fingerprint RX         | GPIO 17   |
| Red LED                | GPIO 26   |
| Green LED              | GPIO 27   |
| SD Card CS             | GPIO 5    |
| SD Card MOSI           | GPIO 23   |
| SD Card MISO           | GPIO 19   |
| SD Card SCK            | GPIO 18   |

Fingerprint Sensor Power: VCC → 5V, GND → GND

## Libraries Required
- Adafruit Fingerprint Sensor Library
- SD (built-in)
- WiFi (built-in)
- HTTPClient (built-in)

## Setup Instructions
1. Install the required libraries in Arduino IDE.
2. Update WiFi credentials and Google Apps Script URL in the code.
3. Format SD card as FAT32.
4. Upload the code.
5. Open Serial Monitor (115200 baud).
6. Type `enroll` to register new users.

## Google Sheets Setup
1. Create a new Google Sheet.
2. Go to **Extensions → Apps Script**.
3. Paste the provided Google Apps Script code.
4. Deploy as Web App (Execute as: Me, Access: Anyone).
5. Copy the Web App URL into the Arduino code.

## Serial Commands
- `enroll` → Register a new fingerprint + name
