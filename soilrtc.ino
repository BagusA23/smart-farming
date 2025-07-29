#include <Wire.h>
#include "RTClib.h"
#include <SoftwareSerial.h>

#define SOIL_PIN A0
#define RELAY_PIN 7

RTC_DS3231 rtc;
SoftwareSerial bt(10, 11); // RX, TX

int soilMoisture;
int threshold = 500;
bool manualMode = false;
bool pompaOn = false;
bool lastPompaState = false; // Untuk tracking perubahan status pompa

void setup() {
  Serial.begin(9600);
  bt.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Matikan pompa

  if (!rtc.begin()) {
    Serial.println("RTC tidak ditemukan!");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC kehilangan daya, set waktu sekarang!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Kirim perintah AT ke HC-05 untuk ganti nama
  delay(1000);
  bt.println("AT");
  delay(1000);
  bt.println("AT+NAME=POMPA_SMART"); // Ganti nama Bluetooth di sini
  delay(1000);

  Serial.println("Sistem siap...");
  bt.println("Bluetooth siap. Kirim 'ON', 'OFF', atau 'AUTO'");
}

void loop() {
  // Baca Bluetooth
  if (bt.available()) {
    String command = bt.readStringUntil('\n');
    command.trim();

    if (command == "ON") {
      if (!manualMode || !pompaOn) { // Hanya print jika ada perubahan
        manualMode = true;
        pompaOn = true;
        DateTime now = rtc.now();
        Serial.print("Mode manual: Pompa ON jam: ");
        Serial.print(now.hour());
        Serial.print(":");
        Serial.print(now.minute());
        Serial.print(":");
        Serial.println(now.second());
        bt.println("Pompa DINYALAKAN (manual)");
      }
    } else if (command == "OFF") {
      if (!manualMode || pompaOn) { // Hanya print jika ada perubahan
        manualMode = true;
        pompaOn = false;
        DateTime now = rtc.now();
        Serial.print("Mode manual: Pompa OFF jam: ");
        Serial.print(now.hour());
        Serial.print(":");
        Serial.print(now.minute());
        Serial.print(":");
        Serial.println(now.second());
        bt.println("Pompa DIMATIKAN (manual)");
      }
    } else if (command == "AUTO") {
      if (manualMode) { // Hanya print jika ada perubahan
        manualMode = false;
        Serial.println("Mode otomatis diaktifkan");
        bt.println("Mode otomatis aktif");
      }
    }
  }

  // Baca sensor soil moisture selalu (untuk semua mode)
  soilMoisture = analogRead(SOIL_PIN);
  
  // Print pembacaan sensor setiap 5 detik untuk semua mode
  static unsigned long lastSensorPrint = 0;
  if (millis() - lastSensorPrint > 5000) {
    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisture);
    if (manualMode) {
      Serial.print(" (Mode: Manual - Pompa ");
      Serial.print(pompaOn ? "ON" : "OFF");
      Serial.println(")");
    } else {
      Serial.println(" (Mode: Auto)");
    }
    lastSensorPrint = millis();
  }

  // Kontrol pompa
  if (manualMode) {
    digitalWrite(RELAY_PIN, pompaOn ? LOW : HIGH);
  } else {
    // Mode otomatis

    bool shouldPumpOn = soilMoisture < threshold;
    
    // Hanya aktifkan pompa jika status berubah atau pompa belum pernah nyala untuk kondisi ini
    if (shouldPumpOn && (lastPompaState != shouldPumpOn)) {
      digitalWrite(RELAY_PIN, LOW); // Pompa nyala
      DateTime now = rtc.now();
      Serial.print("Tanah kering, pompa ON jam: ");
      Serial.print(now.hour());
      Serial.print(":");
      Serial.print(now.minute());
      Serial.print(":");
      Serial.println(now.second());
      
      lastPompaState = true;
      delay(5000); // Pompa nyala selama 5 detik
      
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Pompa dimatikan (otomatis).\n");
      lastPompaState = false;
      delay(10000); // Tunggu 10 detik sebelum cek lagi
    } else if (!shouldPumpOn && (lastPompaState != shouldPumpOn)) {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Tanah lembab. Pompa OFF.");
      lastPompaState = false;
      delay(2000);
    } else {
      // Tidak ada perubahan status, hanya delay singkat
      delay(1000);
    }
  }
}