#include <SPI.h>
#include <LoRa.h>
#include "BluetoothSerial.h"

// Create a Bluetooth Serial object
BluetoothSerial BTSerial;

// Define LoRa pins
#define ss 5
#define rst 14
#define dio0 2

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa + Bluetooth Bridge");

  // Start Bluetooth
  BTSerial.begin("ESP32_LoRa_BT");
  Serial.println("Bluetooth started. Pair and connect.");

  // Setup LoRa
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(500);
  }
  LoRa.setSyncWord(0xAB); // Must match with sender/receiver
  Serial.println("LoRa Initialized Successfully!");
}

void loop() {
  // 🔸 1. Check if LoRa received something
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received LoRa packet: ");
    BTSerial.print("Received LoRa packet: ");

    String LoRaData = "";
    while (LoRa.available()) {
      LoRaData += (char)LoRa.read();
    }

    Serial.print(LoRaData);
    BTSerial.print(LoRaData);

    Serial.print(" | RSSI: ");
    Serial.println(LoRa.packetRssi());
    //BTSerial.print(";");
    BTSerial.println(LoRa.packetRssi());
  }

  // 🔸 2. Check if Bluetooth received something
  if (BTSerial.available()) {
    String BTData = BTSerial.readStringUntil('\n'); // Read until newline
    BTData.trim(); // Remove unwanted spaces/newlines

    if (BTData.length() > 0) {
      Serial.print("Sending via LoRa: ");
      Serial.println(BTData);

      LoRa.beginPacket();
      LoRa.print(BTData);
      LoRa.endPacket();

      Serial.println("LoRa transmission done!");
    }
  }
}
