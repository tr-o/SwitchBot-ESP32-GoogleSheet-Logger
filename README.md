# SwitchBot-ESP32-GoogleSheet-Logger

This repository contains an Arduino sketch and Google Apps Script (GAS) code to connect an ESP32 to SwitchBot Thermometer devices, log temperature, humidity, and battery data, and send the collected data to a Google Spreadsheet.

## Hardware

This project uses the following hardware components:

- [SwitchBot Thermometer](https://www.switch-bot.com/products/switchbot-thermometer-hygrometer): A device for measuring temperature and humidity.
- [ESP32-WROVER-Dev Board](https://www.espressif.com/en/products/devkits/esp32-devkitc): A development board for the ESP32 microcontroller, which connects to the SwitchBot devices and sends the collected data to the Google Spreadsheet.

## Disabling CO2 Sensor

If you don't need to measure CO2 levels, you can simply comment out the related code sections in the main Arduino file. This will disable the CO2 sensor functionality in the project, and the data will not be sent to Google Sheets.

To disable the CO2 sensor, look for the code sections related to the MH-Z19 CO2 sensor (e.g., MHZ19_uart library, initialization, and data reading) and comment them out.

## Arduino Sketch

The Arduino sketch is responsible for connecting to SwitchBot devices using ESP32 and collecting temperature, humidity, and battery data. After collecting the data, it sends the information to the Google Apps Script.

### Dependencies

- WiFi.h
- Arduino.h
- WiFiClientSecure.h
- BLEDevice.h
- BLEUtils.h
- BLEScan.h
- BLEAdvertisedDevice.h
- MHZ19_uart.h (for the CO2 sensor)

### Setup

1. Install the required libraries in your Arduino IDE.
2. Update the sketch with your Wi-Fi SSID and password.
3. Replace the placeholders for the Google Apps Script web app URL.
4. Update the device addresses and UUIDs as needed.
5. Upload the sketch to your ESP32.

## Google Apps Script

The Google Apps Script (GAS) code is responsible for receiving the data sent by the ESP32 and appending it to a Google Spreadsheet.

### Setup

1. Create a new Google Spreadsheet.
2. Click on "Extensions" > "Apps Script".
3. Replace the existing `Code.gs` with the provided GAS code below.
4. Update the Google Spreadsheet URL in the GAS code.
5. Click "Deploy" > "New deployment".
6. Choose "Web app" as the deployment type and set the access to "Anyone".
7. Copy the web app URL and use it in the Arduino sketch.

## Usage
Once both the Arduino sketch and Google Apps Script are set up, the ESP32 will periodically collect data from the connected SwitchBot devices and send it to the Google Spreadsheet using the GAS web app URL. The collected data will be appended to the spreadsheet in a new row.

## References

This project refers to the following resources:

- [ESP32でSwitchBotに接続しGoogleスプレッドシートにデータを送る](https://qiita.com/takeru@github/items/f42381e8482c3bf484e7)
- [MH-Z19 CO2 Sensor Library for UART communication](https://github.com/nara256/mhz19_uart/blob/master/README_jp.md)

