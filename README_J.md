<a href="https://mono-wireless.com/jp/index.html">
    <img src="https://mono-wireless.com/common/images/logo/logo-land.svg" alt="mono wireless logo" title="MONO WIRELESS" align="right" height="60" />
</a>

# spot-thingspeak

**Mono Wireless TWELITE SPOT Thingspeak Client Example**
(*mwings-v1.2.0*)

[![MW-OSSLA](https://img.shields.io/badge/License-MW--OSSLA-e4007f)](LICENSE.md)
[日本語版はこちら](README_J.md)

## 目次

- [概要](#概要)
- [使用方法](#使用方法)
- [依存関係](#依存関係)
- [ライセンス](#ライセンス)

## 概要

TWELITE ARIA の温湿度データを Thingspeak へ送信します。データは Thingspeak の Web ページにてグラフとして閲覧できます。

- 例として、TWELITE ARIA からのパケットを受信します。
- データを `https://api.thingspeak.com/update` へ送信します。
  - 詳細は [Thingspeakのサイト](https://thingspeak.mathworks.com/) をご覧ください。

## 使用方法

1. `config.h` を編集して、Wi-Fi 設定と Write API key を登録してください。
2. スケッチを書き込みます。
3. 目安として30秒以上の送信間隔を設定した TWELITE ARIA を起動してください。

## 依存関係

### TWELITE SPOT 内の TWELITE BLUE

- ファームウェア
  - App_Wings (TWELITE_SPOT) (>= 1.3.2)

### TWELITE SPOT 内の ESP32-WROOM-32

- 開発環境
  - [Arduino IDE](https://github.com/arduino/Arduino) (1.x)
  - [ESP32 Arduino core](https://github.com/espressif/arduino-esp32) (>= 3.1.0)
- ライブラリ
  - [MWings](https://github.com/monowireless/mwings_arduino) (>= 1.2)

## ライセンス

``` plain
Copyright (C) 2024 Mono Wireless Inc. All Rights Reserved.
Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
```
