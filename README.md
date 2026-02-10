# 🧠 Smart Physio System  
### IoT TENS & Patient Monitoring Device

A wearable **IoT-based medical prototype** designed for automated physiotherapy and patient safety monitoring.  
The system detects **muscle fatigue** and **posture abnormalities**, syncs data to the cloud in real time, and **automatically activates a TENS (Transcutaneous Electrical Nerve Stimulation) unit** when required.

---

## 👨‍💻 Developer Information

- **Developer:** Harshavardhan Talap  
- **Project Type:** IoT Medical Prototype  
- **Date:** February 2026  

---

## 📌 Project Overview

The **Smart Physio System** assists patients during rehabilitation by continuously monitoring muscle activity and body posture.

### ❗ Problem
Patients often **over-exert muscles** or maintain **incorrect posture** without realizing it, leading to delayed recovery or injury.

### ✅ Solution
This device:
- Tracks **muscle strain** using a flex sensor  
- Measures **body tilt and movement** using a gyroscope  
- Automatically activates **TENS therapy** when fatigue is detected  
- Logs all data to a **cloud-based mobile app** for medical review

---

## ⭐ Key Features

- **Auto-TENS Therapy**  
  Automatically triggers muscle stimulation (via **TIP122 driver**) when the flex sensor detects fatigue.

- **Fall & Posture Detection**  
  Uses **MPU6050 (Gyroscope + Accelerometer)** to detect unsafe body angles (rolling / falling).

- **Real-Time Cloud Sync**  
  ESP32 pushes live data to **Google Firebase Realtime Database**.

- **Android App Dashboard**  
  Custom Android app (`com.example.flexa`) displays:
  - 🟢 **Safe Status**
  - 🔴 **Unsafe Status**

- **Safety Engineering**
  - Brownout protection  
  - Battery optimization algorithms  
  - Stable operation on a **9V portable power source**

---

## 🛠️ Hardware & Bill of Materials

*Based on the project cost sheet.*

| Component | Specification | Function |
|---------|--------------|---------|
| **Microcontroller** | ESP32-WROOM-32 | Main controller (WiFi + Logic) |
| **Muscle Sensor** | Flex Sensor (2.2 inch) | Detects muscle bending & fatigue |
| **Posture Sensor** | MPU6050 | 6-axis accelerometer & gyroscope |
| **TENS Transformer** | 9-0-9 Step-Up (500mA) | Generates therapeutic high-voltage pulses |
| **Driver Transistor** | TIP122 (Darlington) | High-current switching for transformer |
| **Isolation** | PC817 Optocouplers | Protects ESP32 from voltage spikes |
| **Power Source** | Hi-Watt 9V Battery | Portable power supply |
| **Voltage Regulation** | LM2596 Buck Converter | Steps down 9V to 5V for ESP32 |

---

## 📂 Project Structure

Smart-Physio-System/<br>
├── App/ # Android Studio project (Java/XML)<br>
├── Firmware/ # ESP32 Arduino (.ino) source code<br>
├── Circuit/ # Schematics and wiring diagrams<br>
└── Documentation/ # Cost sheets and project reports<br>
---

## 🚀 How to Run

### 1️⃣ Firmware Setup (ESP32)

1. Open  
   `Firmware/Sketch_for_esp/Sketch_for_esp.ino` in **Arduino IDE**
2. Install required libraries:
   - `Firebase_ESP_Client`
   - `Adafruit_MPU6050`
   - `Wire`
3. Update **WiFi credentials** and **Firebase API Key** in the code
4. Select board: **DOIT ESP32 DEVKIT V1**
5. Upload the firmware

---

### 2️⃣ Android App Setup

1. Open the `/App` folder in **Android Studio**
2. Sync the project with **Gradle**
3. Place `google-services.json` inside the `app/` directory
4. Build and run on:
   - Physical Android device  
   - Android Emulator  

---

## ⚠️ Safety Warnings

- **High Voltage Alert**  
  The TENS unit generates high-voltage pulses.  
  **Do NOT touch electrode output wires** while the device is active.

- **Battery Safety**  
  When powered by a 9V battery, the firmware limits WiFi power consumption to prevent **brownout and voltage drops**.

---

## 📌 Disclaimer

This project is a **prototype for educational and research purposes only**.  
It is **not a certified medical device** and should not be used for clinical treatment without professional supervision.
