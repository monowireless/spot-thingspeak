<a href="https://mono-wireless.com/jp/index.html">
    <img src="https://mono-wireless.com/common/images/logo/logo-land.svg" alt="mono wireless logo" title="MONO WIRELESS" align="right" height="60" />
</a>

# spot-thingspeak

**Mono Wireless TWELITE SPOT Thingspeak Client Example**
(*mwings-v1.2.0*)

[![MW-OSSLA](https://img.shields.io/badge/License-MW--OSSLA-e4007f)](LICENSE.md)
[日本語版はこちら](README_J.md)

## Contents

- [About](#about)
- [Usage](#usage)
- [Dependencies](#dependencies)
- [License](#license)

## About

Send temperature and humidity from TWELITE ARIA to Thingspeak. Data can be monitored as plots in the Thingspeak web page.

- Receive packets from TWELITE ARIA as an example.
- Send data to `https://api.thingspeak.com/update`.
  - For details, see [Thingspeak Web](https://thingspeak.mathworks.com/)

## Usage

1. Modify Wi-Fi configuration and Write API key in the `config.h` file.
2. Upload & Run the sketch.
3. Start TWELITE ARIA in 30s or longer interval.

## Dependencies

### TWELITE BLUE on the TWELITE SPOT

- Firmware
  - App_Wings (TWELITE_SPOT) (>= 1.3.2)

### ESP32-WROOM-32 on the TWELITE SPOT

- Environment
  - [Arduino IDE](https://github.com/arduino/Arduino) (1.x)
  - [ESP32 Arduino core](https://github.com/espressif/arduino-esp32) (>= 3.1.0)
- Libraries
  - [MWings](https://github.com/monowireless/mwings_arduino) (>= 1.2)

## License

``` plain
Copyright (C) 2024 Mono Wireless Inc. All Rights Reserved.
Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
```
