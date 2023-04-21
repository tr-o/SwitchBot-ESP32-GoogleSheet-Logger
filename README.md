# SwitchBot-ESP32-GoogleSheet-Logger

This repository contains an Arduino sketch and Google Apps Script (GAS) code to connect an ESP32 to SwitchBot devices, log temperature, humidity, and battery data, and send the collected data to a Google Spreadsheet.

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

