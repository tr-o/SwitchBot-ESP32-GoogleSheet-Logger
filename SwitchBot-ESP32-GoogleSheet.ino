#include <WiFi.h>
#include "Arduino.h"
#include <WiFiClientSecure.h>

#ifndef WIFI_SSID
#define WIFI_SSID "**" // WiFi SSID (2.4GHz only)
#endif

#ifndef WIFI_PASSPHREASE
#define WIFI_PASSPHREASE "**" // WiFiパスワード
#endif

const char* host = "script.google.com";
const int httpsPort = 443;

//Google Apps ScriptのウェブアプリケーションURL
String url = "**";

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

BLEAddress* addr01 = new BLEAddress("**:**:**:**:**:**");
BLEAddress* addr02 = new BLEAddress("**:**:**:**:**:**");
BLEUUID serviceUUID = BLEUUID("cba20d00-224d-11e6-9fb8-0002a5d5c51b");
BLEUUID serviceDataUUID = BLEUUID("00000d00-0000-1000-8000-00805f9b34fb");
const int DEVICE_COUNT = 2; // デバイスの数に応じて値を変更してください

struct DeviceData {
  int battery;
  float temperature;
  int humidity;
  bool newDataAvailable = false;
};

DeviceData deviceData[2];

#include <MHZ19_uart.h>
const int rx_pin = 32; //Serial rx pin no
const int tx_pin = 33; //Serial tx pin no
MHZ19_uart mhz19;


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    int deviceIndex = -1;

    if (advertisedDevice.getAddress().equals(*addr01)) {
      deviceIndex = 0;
    } else if (advertisedDevice.getAddress().equals(*addr02)) {
      deviceIndex = 1;
    } else {
      return;
    }

    if (!advertisedDevice.haveServiceUUID()) return;
    if (!advertisedDevice.getServiceUUID().equals(serviceUUID)) return;

    if (!advertisedDevice.haveServiceData()) return;

    std::string s = advertisedDevice.getServiceData();

    const char* servicedata = s.c_str();
    deviceData[deviceIndex].battery = servicedata[2] & 0b01111111;
    bool isTemperatureAboveFreezing = servicedata[4] & 0b10000000;
    deviceData[deviceIndex].temperature = (servicedata[3] & 0b00001111) / 10.0 + (servicedata[4] & 0b01111111);
    if (!isTemperatureAboveFreezing) {
      deviceData[deviceIndex].temperature = -deviceData[deviceIndex].temperature;
    }
    deviceData[deviceIndex].humidity = servicedata[5] & 0b01111111;

    deviceData[deviceIndex].newDataAvailable = true;
  }
};

void printData(int deviceIndex) {
  Serial.print("Device ");
  Serial.print(deviceIndex);
  Serial.print(" - Battery: ");
  Serial.print(deviceData[deviceIndex].battery);
  Serial.print("%, Temperature: ");
  Serial.print(deviceData[deviceIndex].temperature);
  Serial.print("°C, Humidity: ");
  Serial.print(deviceData[deviceIndex].humidity);
  Serial.println("%");
}

void sendAllDevices2GAS() {
  String payload = "";
  payload += "DEVICE_COUNT=" + String(DEVICE_COUNT) + "&";

  int co2ppm = mhz19.getCO2PPM();
  int temp = mhz19.getTemperature();

  for (int deviceIndex = 0; deviceIndex < DEVICE_COUNT; deviceIndex++) {
   
    payload += "Temp" + String(deviceIndex) + "=" + String(deviceData[deviceIndex].temperature) + "&";
    payload += "Hum" + String(deviceIndex) + "=" + String(deviceData[deviceIndex].humidity) + "&";
    payload += "Batt" + String(deviceIndex) + "=" + String(deviceData[deviceIndex].battery) + "&";
  }
  payload += "CO2=" + String(co2ppm) + "&";

  payload.remove(payload.length() - 1); // Remove the last '&'
  Serial.println(payload);
  send2GAS(payload);
}

void send2GAS(const String &payload) {
  WiFiClientSecure client;
  Serial.print("Connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }

  // HTTPリクエストを送信
  client.print("POST " + url + " HTTP/1.1\r\n");
  client.print("Host: " + String(host) + "\r\n");
  client.print("Content-Type: application/x-www-form-urlencoded\r\n");
  client.print("Content-Length: " + String(payload.length()) + "\r\n");
  client.print("Connection: close\r\n");
  client.print("\r\n");
  client.print(payload);
  Serial.print("Connecting to ");

  // サーバーからの応答を読み取る
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  String response = client.readStringUntil('\n');
  Serial.println("Response:");
  Serial.println(response);
  client.stop();
}

void send2GAS_delete(int deviceIndex) {
  WiFiClientSecure client;
  Serial.print("Connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }
;
  String payload =  "deviceIndex=Device_" + String(deviceIndex);
  payload += "&Temperature_dev"+ String(deviceIndex) + "=" + String(deviceData[deviceIndex].temperature); 
  payload += "&Humidity_dev" + String(deviceIndex) + "=" + String(deviceData[deviceIndex].humidity);
  payload += "&Battery_dev" + String(deviceIndex) + "="  + String(deviceData[deviceIndex].battery);

  
  // HTTPリクエストを送信
  client.print("POST " + url + " HTTP/1.1\r\n");
  client.print("Host: " + String(host) + "\r\n");
  client.print("Content-Type: application/x-www-form-urlencoded\r\n");
  client.print("Content-Length: " + String(payload.length()) + "\r\n");
  client.print("Connection: close\r\n");
  client.print("\r\n");
  client.print(payload);
  Serial.print("Connecting to ");
  
  // サーバーからの応答を読み取る
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  String response = client.readStringUntil('\n');
  Serial.println("Response:");
  Serial.println(response);
  client.stop();
}

#include "esp_deep_sleep.h"
void startDeepSleep(int time_in_second){
    Serial.println("go to sleep");
    esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_deep_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF); 
    esp_deep_sleep_enable_timer_wakeup(time_in_second * 1000 * 1000);  // wakeup(restart) after 20secs
    esp_deep_sleep_start();
}

void connect_wifi()
{
  int connectionFailures = 0; // Counter for failed connection attempts
  const int maxFailures = 30; // Maximum number of allowed failed attempts

  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.disconnect(true);
  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASSPHREASE);

  while (WiFi.status() != WL_CONNECTED)
  {
    connectionFailures++; // Increment the failure counter

    if (connectionFailures > maxFailures) {
      Serial.println("Exceeded maximum connection attempts. Restarting...");
      ESP.restart(); // Restart the ESP32
    }

    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi Connected");
  Serial.printf("IPv4: %s", WiFi.localIP().toString().c_str());
}

void setupMHZ(){
  mhz19.begin(rx_pin, tx_pin);
  mhz19.setAutoCalibration(false);
  mhz19.setHardwareSerialNo(2);
  Serial.println("MH-Z19 is warming up now.");
  delay(3 * 1000); //
}

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");
  connect_wifi();
  setupMHZ();
 

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);


}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults();

  if (deviceData[0].newDataAvailable && deviceData[1].newDataAvailable) {
    printData(0);
    printData(1);
    sendAllDevices2GAS();
    deviceData[0].newDataAvailable = false;
    deviceData[1].newDataAvailable = false;
    delay(5000);
    startDeepSleep(300);   
  }
  


}
