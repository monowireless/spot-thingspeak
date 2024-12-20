// -*- coding:utf-8-unix -*-
// Based on spot-httpbin

// Arduino / ESP libraries
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>

// Third-party libraries

// Mono Wireless TWELITE Wings API
#include <MWings.h>

// Local configs
#include "config.h"

// Types
struct DataFromAria {
    uint32_t serialId;
    uint8_t logicalId;
    uint16_t supplyVoltage;
    uint8_t linkQuality;
    int16_t temp100x;
    uint16_t humid100x;
};

// TWELITE on the SPOT
static const int RST_PIN = 5;
static const int PRG_PIN = 4;
static const int LED_PIN = 18;

static const int8_t RX1_PIN = 16;
static const int8_t TX1_PIN = 17;

// Interfaces
static WiFiClientSecure client;

// Global variables
static DataFromAria LatestDataFromAria;
static bool IsThereNewDataFromAria;

// System
void anotherLoopForTWELITE();

// Initialization
void initTWELITE();
void initWiFi();

// TWELITE
void onAppAriaPacket(const ParsedAppAriaPacket& packet);

// App
void sendAriaData(const DataFromAria& data);

// System Setup
void setup() {
    Serial.begin(115200);

    initTWELITE();
    initWiFi();

    // Attach another loop function for TWELITE
    // Note: Core 0 is also used for the WiFi task, which priority is 19 (ESP_TASKD_EVENT_PRIO - 1)
    xTaskCreatePinnedToCore(
        [](void *params) {
            while (true) {
                anotherLoopForTWELITE();
                vTaskDelay(1); // IMPORTANT for Watchdog
            }
        },
        "Task for anotherLoopForTWELITE()", 8192, nullptr, 18, nullptr,
        0); // Priority is 18 (lower than WiFi)
}

// System loop
void loop() {
    static uint32_t lastTimeReconnected = 0;
    if (WiFi.status() == WL_CONNECTED) {
        // Regular operations
        // Check for new data
        if (IsThereNewDataFromAria) {
            IsThereNewDataFromAria = false; // Clear first; data is updated on another thread
            DataFromAria data = LatestDataFromAria; // Now, the buffer is open for incoming data
            sendAriaData(data);
        }
    } else if (millis() - lastTimeReconnected > RECONNECT_MIN_INTERVAL * 1000) {
        // Lost connection, reconnect periodically
        Serial.println("Disconnected. Reconnecting to WiFi...");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        lastTimeReconnected = millis();
    }
    // Reboot every x interval
    if (millis() > REBOOT_INTERVAL * 1000) {
        Serial.println("Rebooting...");
        ESP.restart();
    }
}

// Another loop procedures; Priority is higher than loop()
void anotherLoopForTWELITE() {
    Twelite.update();
}

// Initialize TWELITE
void initTWELITE() {
    Serial2.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);
    if (Twelite.begin(Serial2, LED_PIN, RST_PIN, PRG_PIN, TWE_CHANNEL, TWE_APP_ID)) {
        Serial.println("Started TWELITE.");
    }
    // Attach event handlers to process packets
    Twelite.on(onAppAriaPacket);
}

// Initialize WiFi
void initWiFi() {
    Serial.print("\nConnecting to the WiFi network ");
    Serial.print(WIFI_SSID);
    Serial.println("...");
    // Begin
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // Wait for connection
    Serial.print("Connecting.");
    while (WiFi.status() != WL_CONNECTED) {
        static int count = 0;
        Serial.print('.');
        delay(500);
        // Retry every 5 seconds
        if (count++ % 10 == 0) {
            WiFi.disconnect();
            WiFi.reconnect();
            Serial.print('!');
        }
    }
    Serial.println("\nConnected!");
    // Set Root CA certificate
    client.setCACert(CA_CERT);
}

// Process on receiving a packet from App_ARIA
void onAppAriaPacket(const ParsedAppAriaPacket& packet)
{
    // Store data
    LatestDataFromAria.serialId = packet.u32SourceSerialId;
    LatestDataFromAria.logicalId = packet.u8SourceLogicalId;
    LatestDataFromAria.supplyVoltage = packet.u16SupplyVoltage;
    LatestDataFromAria.linkQuality = packet.u8Lqi;
    LatestDataFromAria.temp100x = packet.i16Temp100x;
    LatestDataFromAria.humid100x = packet.u16Humid100x;
    IsThereNewDataFromAria = true;
}

// Send data
void sendAriaData(const DataFromAria& data)
{
    static uint32_t lastTimeRequested = 0;
    if (millis() - lastTimeRequested > SEND_MIN_INTERVAL * 1000 or lastTimeRequested == 0) {
        Serial.println("Connecting to the server...");
        if (not client.connect(SERVER_HOST, SERVER_PORT, CONNECT_TIMEOUT * 1000)) {
            Serial.println("Connection failed!");
            char err_buf[100];
            if (client.lastError(err_buf, 100) < 0) {
                Serial.println(err_buf);
            } else {
                Serial.println("Connection error");
            }
        } else {
            Serial.println("Connected to the server!");
            // Make a query string for the Channel on the ThingSpeak
            char queries[QUERIES_MAX_LENGTH+1];
            snprintf(queries, sizeof(queries),
                     "api_key=%s&field1=%s&field2=%s&field3=%s&field4=%d",
                     // Write API key for the Channel
                     WRITE_API_KEY,
                     // Field 1: Temperature
                     String(data.temp100x / 100.0f, 2),
                     // Field 2: Humidity
                     String(data.humid100x / 100.0f, 2),
                     // Field 3: Supply Voltage
                     String(data.supplyVoltage / 1000.0f, 2),
                     // Field 4: Link Quality
                     data.linkQuality);

            // Send a request
            client.println(String("GET https://") +
                           SERVER_HOST +
                           String("/update?") +
                           queries +
                           String(" HTTP/1.1"));
            client.println("Accept: */*");
            client.println(String("Host: ") + SERVER_HOST);
            client.println("Connection: close");
            client.println();
            uint32_t timeSentRequest = millis();

            // Handle a response
            while (client.connected()) {
                String line = client.readStringUntil('\n');
                if (line == "\r") {
                    Serial.println("Headers received");
                    break;
                }
                if (millis() - timeSentRequest > REQUEST_TIMEOUT * 1000) {
                    Serial.println("Request was timed out");
                    break;
                }
            }
            while (client.available()) {
                char c = client.read();
                Serial.write(c);
            }
            client.stop();
            Serial.println("");
        }
        lastTimeRequested = millis();
    } else {
        Serial.println("Requests are too frequently; skip.");
    }
}

/*
 * Copyright (C) 2024 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */
