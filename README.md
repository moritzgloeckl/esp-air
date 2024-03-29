# ESPAir - Indoor air quality monitor

A low-cost, smart, indoor air quality monitor based on the SM300D2 sensor module, an ESP32 and ESPHome.io for use with Home Assistant.

## Features

The SM300D2 module provides the following sensors:

| Sensor | Range          | Accuracy |
|--------|----------------|----------|
| CO2    | 400 - 5000 ppm | ± 50ppm  |
| Formaldehyde | 1 - 1000 µg/m³ | ? |
| TVOC    | 0 - 2000 µg/m³ | Calculated |
| PM2.5  | 5 - 100 µg/m³     | ± 10% |
| PM10  | 5 - 100 µg/m³     | ± 10% |
| Temperature  | -40 to +125 °C     | ± 0.5 °C |
| Humidity  | 0 - 100%     | ± 3% |

It should be noted that the CO2 sensor is in fact an eCO2 (equivalent carbon dioxide) sensor, which means that the CO2 is not measured, but rather calculated (or approximated) from the TVOC reading.

## Advantages

- Easy setup/use
- Consistent values
- Inexpensive (when compared to getting each module seperately)
- No assembly necessary
- Relatively stable and accurate 

## Bill of Materials

| Part        | Price  | Link     |
|-------------|--------|----------|
| SM300D2     | ~ 40 € | [Click me](https://www.banggood.com/SM300D2-7-in-1-PM2_5-+-PM10-+-Temperature-+-Humidity-+-CO2-+-eCO2-+-TVOC-Sensor-Tester-Detector-Module-for-Air-Quality-Monitoring-p-1681079.html) |
| ESP32 Board | ~ 5 €  | [Click me](https://www.banggood.com/ESP32-Development-Board-WiFi+bluetooth-Ultra-Low-Power-Consumption-Dual-Cores-ESP-32-ESP-32S-Board-p-1109512.html) |

Cables with JST connectors where included when ordering from above shop in late January 2021.

## Connection

The SM300D2 module is connected to the ESP32 via UART. The interface on the SM300D2 module is defined as:

| Pin | Name          | Description |
|--------|----------------|----------|
| 1    | 5V | Power  |
| 2  | T(X)D     | Data output |
| 3  | N/A     | Not in use |
| 4  | G(N)D     | Power ground |

The module uses:

- Baud rate: 9600
- Data bits: 8 bits
- Stop bit: 1 bit
- Check digit: None

To connect the module to the ESP32, I cut the included cable with the JST connector in half, and soldered female jumper wires onto it, including heatshrinks.

Connect the modules 5V to VIN on the ESP32, GND to GND, and the TD to the RX2 pin.

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

1. Calculate the checksum, to see if no transmission errors occured. Sum up all values except the checksum: `0x3C + 0x02 + 0x08 + 0xFC + 0x00 + 0x79 + 0x01 + 0xD7 + 0x00 + 0x13 + 0x00 + 0x22 + 0x1B + 0x03 + 0x30 + 0x02 = 0x18` and check if the check sum is equal to the sum.
2. Calculate CO2: `Byte03 * 256 + Byte04 = 0x08 * 256 + 0xFC = 2300` (ppm)
2. Calculate Formaldehyde: `Byte05 * 256 + Byte06 = 0x00 * 256 + 0x79 = 121` (µg/m³)
2. Calculate TVOC: `Byte07 * 256 + Byte08 = 0x01 * 256 + 0xD7 = 471` (µg/m³)
3. Calculate PM2.5: `Byte09 * 256 + Byte10 = 0x00 * 256 + 0x13 = 19` (µg/m³)
4. Calculate PM10: `Byte11 * 256 + Byte12 = 0x00 * 256 + 0x22 = 34` (µg/m³)
5. Temperature: `Byte13 + Byte14 * 0.1 = 0x1B + 0x03 * 0.1 = 27.3` (°C)
6. Humidity: `Byte15 + Byte16 * 0.1 = 0x30 + 0x02 * 0.1 = 48.2` (% RH)

## Debugging the sensor module

The sensor module may be debugged and its values read, using any UART-capable device, such as a USB-TTL adapter or even an Arduino/Raspberry Pi. Hook up the 5V to the 5V of your adapter, GND to GND and TX of the module to the RX of the UART adapter.

The serial connection tool `minicom` may be used for displaying the values, make sure to set the correct baud rate and stop and parity settings. Sample command:

`minicom --device /dev/cu.usbserial-141220 -H -w`

Use the `-H` flag to enable hex output and the `-w` flag to enable automatic line wrap.

Furthermore, this repository includes a Python script which can be used to debug the module on your device.

In order to debug the actual air quality values and see actual changes there are some options available. I tried flooding the sensor with a CO2 bottle (Sodastream) but that didn't do much, because the sensor does not detect CO2 directly but calculates its value. What works way better is using a pen with solvents like a Sharpie, Edding or other permanent markers and waving it near the sensors and above the fan. You'll see within about 30 seconds the values will go through the roof, e.g. CO2 will be up to 1700ppm to 2000ppm. Then you'll know your sensor is working.

## Other infos

The SM300D2 module (or rather its onboard sensors) needs some time (at least 48h of continued use) to "burn in" before reporting correct values. For example, right after unboxing, the module reported an indoor temperature of 26.5 °C - well above any other thermometer which all claimed the correct 22.5 °C. After 5 hours the module already showed a more accurate reading of 23.5 °C (without an actual temperature change).

## Other sensors within this sensor family

I've noticed there are some newer versions of this sensors available via Aliexpress. They seem to be designed by the same manufacturing company, judging by their UART output description which stayed more or less the same. I've found three new models with model codes:

- SM400D2
- SM400D2MOD
- MAQ7A-US

Should anyone have any of these devices I'd be interested in their review. Feel free to open an issue here. 

## Recommended indoor guidelines (various sources)

Following values are considered acceptable for private, indoor air. 

- CO2 should be lower than 1000 ppm ([Source](https://www.dhs.wisconsin.gov/chemical/carbondioxide.htm))
- Formaldehyde should be lower than 50 µg/m³ ([Source](https://www.canada.ca/en/health-canada/services/environmental-workplace-health/reports-publications/air-quality/formaldehyde-indoor-air-environment-workplace-health.html#a3))
- TVOC recommended levels will need to be analysed.
- PM2.5 should be lower than 25 µg/m³ ([Source](https://www.eea.europa.eu/themes/air/air-quality-concentrations/air-quality-standards))
- PM10 should be lower than 50 µg/m³ ([Source](https://www.eea.europa.eu/themes/air/air-quality-concentrations/air-quality-standards))
- Temperature should be between 18 and 24 °C ([Source](https://apps.who.int/iris/rest/bitstreams/1161792/retrieve#page=54))
- Humidity should be between 30 and 50 % RH ([Source](https://www.hvac.com/faq/recommended-humidity-level-home/))

These values should be read as suggestions for long-term indoor exposure.

## Disclaimer

This hardware/software product is only for private use. No medical advice is given whatsoever. Please consult your doctor first. This is only for information purposes. I am not affiliated with any shops/websites/vendors/hardware producers whatsoever.
