# ESP32 Soil Moisture Monitor

A minimal soil moisture monitor using an ESP32 and a bare 2-pin resistive probe. It serves a live web dashboard on your local Wi-Fi: moisture percentage, a color-changing bar, and a status message.

No resistors, amplifier board, or display needed.

## Parts

- ESP32 dev board
- Resistive soil moisture probe (2 pins, "Soil moisture detection" on the board)
- 2 jumper wires
- USB cable and 5V power

## Wiring

| Probe pin | ESP32 |
|-----------|-------|
| Pin A     | GPIO 4 (powers the probe only while measuring) |
| Pin B     | GPIO 32 (analog read with internal pull-down) |

The two probe pins are interchangeable.

## Setup

1. Open `soil_moisture_esp32.ino` in the Arduino IDE (ESP32 board support installed).
2. Set your Wi-Fi name and password at the top of the file:
   ```cpp
   const char* ssid = "YOUR_WIFI";
   const char* pass = "YOUR_PASSWORD";
   ```
3. Select your ESP32 board, upload, and open Serial Monitor at 115200 baud.
4. Open `http://soil.local` on a phone or computer on the same Wi-Fi. If `.local` doesn't work (some Android devices), use the IP address printed in Serial Monitor.

## Calibration

The defaults are only guesses; every probe reads differently.

1. With the probe in dry air, note the `raw` value (on the page or in Serial Monitor) and set it as `DRY`.
2. Dip only the two prongs in water and note the `raw` value. Set it as `WET`.
3. Re-upload.

## Status levels

| Moisture | Status |
|----------|--------|
| Below 30% | Dry, water me |
| 30-70% | Happy |
| Above 70% | Very wet |

## Notes

- The probe is powered only during each reading (once per second) to slow down corrosion. Resistive probes still wear out over time.
- Keep the header and board dry. Only the two prongs should go in soil, up to the black edge of the board.
- Without an external resistor, readings are small and a bit noisy. A 10kΩ resistor from GPIO 32 to GND gives more stable readings.
- The page only works while your device is on the same Wi-Fi as the ESP32.

## API

`GET /data` returns JSON:

```json
{"raw": 1234, "percent": 56}
```

## Before uploading to GitHub

Make sure your real Wi-Fi name and password are not in the committed file.
