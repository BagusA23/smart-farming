# 🌱 Smart Farming Arduino Project

🚀 *Automated Soil Moisture + RTC + Multi-Relay Control System*  
A simple yet powerful smart agriculture system built using Arduino, DHT11, RTC, relays, and more! This project automates irrigation and notification systems based on real-time environmental conditions 🌦️.

---

## 🧠 Features

🔋 **Auto & Manual Pump Control**  
💧 Reads **soil moisture** and activates **relay-controlled water pump** accordingly. Also supports manual mode via Bluetooth.

⏰ **RTC DS3231 Integration**  
Schedules operations using Real-Time Clock (RTC) for precise timing.

🌡️ **Multi-Relay Output**  
Controls multiple devices: water pumps, alarms, and drainage systems using relays.

📱 **Bluetooth Mode**  
Switch between auto/manual mode via serial/Bluetooth terminal (use HC-05/HC-06).

🖥️ **LCD Display (I2C)**  
Live data feedback for temperature, humidity, and pump status.

---

## ⚙️ Hardware Requirements

| Component         | Description                       |
|------------------|-----------------------------------|
| Arduino Uno/Nano | Main microcontroller               |
| DHT11            | Temperature & humidity sensor      |
| RTC DS3231       | Real-time clock module             |
| Soil Moisture    | Analog soil moisture sensor        |
| Relay Module     | Controls pump/alarm/drainage       |
| LCD 16x2 (I2C)   | Display module                     |
| Bluetooth HC-05  | For wireless manual control        |
| Jumper Wires     | For all connections                |

---

## 🗂️ File Structure

| File Name       | Description                                     |
|----------------|-------------------------------------------------|
| `soilrtc.ino`  | Basic RTC + Soil Moisture + Pump + Bluetooth    |
| `farming.ino`  | Enhanced Smart Farming: DHT11 + LCD + 3 Relays  |

---

## 🔌 Wiring Overview

### Relay (3 channel):
- IN1 → Water pump
- IN2 → Alarm/Warning
- IN3 → Drainage system  
VCC → 5V | GND → GND | COM → Device Power Line

### Soil Moisture:
- A0 → Analog pin  
- VCC, GND as usual

### RTC DS3231:
- SDA → A4 | SCL → A5 (for Uno)

### DHT11:
- Data → Pin 7

### LCD I2C:
- SDA → A4 | SCL → A5 (for Uno)

### Bluetooth HC-05:
- TX → Pin 11 | RX → Pin 10 (via voltage divider)

---

## 🧪 How It Works

1. **Auto Mode**:  
   - If soil moisture < threshold → pump ON.
   - Otherwise → pump OFF.
   - Relay status and DHT11 data shown on LCD.

2. **Manual Mode** *(via Bluetooth)*:  
   - Send `ON` to activate pump.
   - Send `OFF` to deactivate pump.
   - Send `AUTO` to switch back to automatic.

---

## 🛠️ Setup

1. Upload `.ino` file using Arduino IDE.
2. Open Serial Monitor @9600 baud.
3. Adjust threshold values if needed.
4. Enjoy your smart irrigation system! 🌾

---

## 🧠 Future Improvements

- Add Blynk/IoT support 🌐  
- Use capacitive soil sensors for better accuracy 🌱  
- Add solar-powered battery support 🔋☀️  
- Expand with water level sensors or pH sensors

---

## 🧑‍💻 Author

Made with ❤️ by [Bagus] – Teknik Informatika, 2025 🌟  
Always learning, always building. Let's automate the farm!

---

## 📸 Screenshots (Coming Soon...)

