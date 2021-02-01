# ESPAir

A low-cost smart indoor air quality monitor based on the SM300D2 sensor module, an ESP32 and ESPHome.io for use with Home Assistant.

## Sensors

The SM300d2 module contains the following sensors:

| Sensor | Range          | Accuracy |
|--------|----------------|----------|
| CO2    | 400 - 5000 ppm | ± 50ppm  |
| Formaldehyde | 1 - 1000 ug | ? |
| TVOC    | 0 - 2000 ug | ? |
| PM2.5  | 5 - 100 ug     | ± 10% |
| PM10  | 5 - 100 ug     | ± 10% |
| Temperature  | -40 - +125 °C     | ± 0.5 °C |
| Humidity  | 0 - 100%     | ± 3% |

## Bill of Materials

| Part        | Price  | Link     |
|-------------|--------|----------|
| SM300d2     | ~ 40 € | [Click me](https://www.banggood.com/ESP32-Development-Board-WiFi+bluetooth-Ultra-Low-Power-Consumption-Dual-Cores-ESP-32-ESP-32S-Board-p-1109512.html?rmmds=myorder&cur_warehouse=CN) |
| ESP32 Board | ~ 5 €  | [Click me](https://www.banggood.com/SM300D2-7-in-1-PM2_5-+-PM10-+-Temperature-+-Humidity-+-CO2-+-eCO2-+-TVOC-Sensor-Tester-Detector-Module-for-Air-Quality-Monitoring-p-1681079.html?cur_warehouse=CN&rmmds=search) |

## Connection

The SM300d2 module is connected to the ESP32 via UART. The interface on the SM300d2 module is defined as:

| Pin | Name          | Description |
|--------|----------------|----------|
| 1    | 5V | Power  |
| 2  | TXD     | Date output |
| 3  | N/A     | Not in use |
| 4  | GND     | Power ground |

The module uses:

Baud rate: 9600
Data bits: 8 bits
Stop bit: 1 bit
Check digit: None

When connected to the UART the module provides outputs in the following format:

`3C 02 08 FC 00 79 01 D7 00 13 00 22 1B 03 30 02 18`

This output is generated so that:

- B01 - always 0x3C
- B02 - always 0x02
- B03 - eCO2 high byte
- B04 - eCO2 low byte
- B05 - eCH2O high byte
- B06 - eCH2O low byte
- B07 - TVOC high byte
- B08 - TVOC low byte
- B09 - PM2.5 high byte
- B10 - PM2.5 low byte
- B11 - PM10 high byte
- B12 - PM10 low byte
- B13 - Temperature pre-decimal
- B14 - Temperature decimal
- B15 - Humidity pre-decimal
- B16 - Humidity decimal
- B17 - Checksum

Following the above example output, the output needs to be interpreted like this:

1. Calculate the checksum, to see if no transmission errors occured. Sum up all values except the checksum: `0x3C + 0x02 + 0x08 + 0xFC + 0x00 + 0x79 + 0x01 + 0xD7 + 0x00 + 0x13 + 0x00 + 0x22 + 0x1B + 0x03 + 0x30 + 0x02 = 0x318` and check if the check sum fits into the sum.
2. Calculate CO2: `Byte03 * 256 + Byte04 = 0x08 * 256 + 0xFC = 2300` (ppm)
2. Calculate Formaldehyde: `Byte05 * 256 + Byte06 = 0x00 * 256 + 0x79 = 121` (ug/m3)
2. Calculate TVOC: `Byte07 * 256 + Byte08 = 0x01 * 256 + 0xD7 = 471` (ug/m3)
3. Calculate PM2.5: `Byte09 * 256 + Byte10 = 0x00 * 256 + 0x13 = 19` (ug/m3)
4. Calculate PM10: `Byte11 * 256 + Byte12 = 0x00 * 256 + 0x22 = 34` (ug/m3)
5. Temperature: `Byte13 + Byte14 * 0.1 = 0x1B + 0x03 * 0.1 = 27.3` (°C)
6. Humidity: `Byte15 + Byte16 * 0.1 = 0x30 + 0x02 * 0.1 = 48.2` (% RH)

## Recommended indoor guidelines (various sources)

Following values are considered acceptable for private, indoor air. 

- CO2 should be lower than 1000 ppm ([Source](https://www.dhs.wisconsin.gov/chemical/carbondioxide.htm))
- Formaldehyde should be ([Source]())
- TVOC should be  ([Source]())
- PM2.5 should be lower than 25 ug/m3 ([Source](https://www.eea.europa.eu/themes/air/air-quality-concentrations/air-quality-standards))
- PM10 should be lower than 50 ug/m3 ([Source](https://www.eea.europa.eu/themes/air/air-quality-concentrations/air-quality-standards))
- Temperature should be between 18 and 24 °C ([Source](https://apps.who.int/iris/rest/bitstreams/1161792/retrieve#page=54))
- Humidity should be between 30 and 50 % RH ([Source](https://www.hvac.com/faq/recommended-humidity-level-home/))
