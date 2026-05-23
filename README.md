---
publishDate: 2026-05-14T00:00:00Z
title: Neo-Sentry 1.0 — Touchless Neonatal Intelligence for Affordable Critical Care
excerpt: A low-cost smart neonatal incubator ecosystem combining IoT automation, atmospheric intelligence, and aseptic gesture control for safer neonatal survival.
image: neo-sentry-cover.jpg
tags:
  - healthcare-iot
  - neonatal-care
  - esp32
  - smart-healthcare
  - embedded-systems
---

> “Because survival should never depend on geography or hospital budgets.”

---

## Acknowledgements

We extend our deepest gratitude to the medical professionals and hardware communities who provided invaluable insights during the research phase. A special thanks to the MYOSA platform for fostering open-source innovation, and to the open-source creators behind the libraries that made this ecosystem possible.

---

## Overview

### The Silent Crisis in Neonatal Care

Every year, millions of premature infants are born into resource-constrained environments. In these critical first weeks of life, a stable, sterile environment is not a luxury—it is the difference between life and death. 

Traditional commercial incubators cost upwards of **₹1,50,000 to ₹7,00,000** (₹1.5–7 Lakhs equivalent in local clinical contexts), making them inaccessible to thousands of rural clinics and developing healthcare centers. Furthermore, the physical interaction required to operate standard medical equipment constantly introduces the risk of cross-contamination—a leading cause of Healthcare-Associated Infections (HAIs) in neonatal intensive care units (NICUs).

### Enter Neo-Sentry 1.0

Neo-Sentry is an uncompromising leap forward in frugal medical engineering. We have reimagined the neonatal incubator not as a piece of static hardware, but as an **intelligent, responsive ecosystem**. 

By leveraging the computational power of the ESP32 microarchitecture, advanced optical proximity sensing, and cloud-native telemetry, Neo-Sentry provides **real-time atmospheric stabilization** and **aseptic, touchless control**.

The financial paradigm shift? **₹2,630.**
Technology built to protect premature lives, democratized for the world.

---

## Demo / Examples

### Images

<p align="center">
<img src="/neo-sentry-system-architecture.jpg" width="800"><br/>
<i>The Neo-Sentry 1.0 Core Control Unit: A sleek, 3D-printable housing protecting the ESP32 intelligence.</i>
</p>

<p align="center">
<img src="/neo-sentry-gesture-interface.jpg" width="800"><br/>
<i>Aseptic Interaction: A nurse adjusting thermal parameters using intuitive, touchless hand gestures.</i>
</p>

<p align="center">
<img src="/neo-sentry-telemetry-dashboard.jpg" width="800"><br/>
<i>Real-time atmospheric telemetry and push notifications delivered directly to a physician's mobile device via ntfy.sh.</i>
</p>

### Videos

<video controls width="100%">
<source src="/neo-sentry-cinematic-trailer.mp4" type="video/mp4">
</video>

---

## Features (Detailed)

Neo-Sentry is engineered around four core pillars of survival: **Asepsis, Stability, Awareness, and Affordability.**

### **1. Live Wi-Fi Connectivity**

Neo-Sentry establishes a continuous, stable wireless connection to facility networks with optimized low-RF transmission profiles. The ESP32 connects to hospital Wi-Fi infrastructure while maintaining persistent connectivity for real-time data streaming and remote monitoring.

### **2. Live OLED Telemetry & Monitoring**

The onboard OLED display continuously visualizes:
- Real-time temperature readings
- Patient stillness tracking
- Active alarm states
- System connectivity status

This provides instant bedside feedback without requiring external monitoring devices.

### **3. Automated Climate Hardware Switching**

Neo-Sentry dynamically activates:
- A cooling fan
- A heating LED

whenever incubator temperatures breach configured clinical boundaries.

The automation system maintains environmental stability using threshold-based control logic to ensure safe neonatal thermal conditions.

### **4. Progressive Hardware Buzzer Alarm**

If patient stillness exceeds a 14-second threshold, Neo-Sentry triggers a progressive physical buzzer alarm pulsing at 400ms intervals.

This acts as an immediate bedside fail-safe alert mechanism independent of internet connectivity.

### **5. Aseptic Optical Alarm Suppression**

Medical staff can instantly suppress active alarms without physical contact by casting a flashlight beam over the optical sensor.

This touchless interaction minimizes contamination risk inside sterile neonatal environments.

### **6. Real-Time Telegram Bot Dispatches**

Neo-Sentry broadcasts:
- Temperature anomalies
- Stillness tracking alerts
- Alarm state transitions
- System status notifications

directly to healthcare staff through Telegram bot integration for remote monitoring access.

### **7. Live Cloud Graphing via ThingSpeak**

Environmental telemetry is streamed to ThingSpeak every 20 seconds across multiple live cloud fields, enabling:
- Continuous monitoring
- Historical trend analysis
- Remote visualization dashboards
- Browser-accessible environmental analytics

### **8. Automated Google Sheets Logging**

All sensor telemetry is automatically appended into cloud-hosted Google Sheets with synchronized timestamps, generating a continuously updating historical monitoring database.

### **9. Self-Hosted Offline Fallback Network**

For facilities without internet infrastructure, Neo-Sentry provisions a standalone local Wi-Fi hotspot (`192.168.4.1`) featuring:
- Live telemetry
- Interactive graphs
- Real-time alerts
- Local monitoring dashboards

This ensures uninterrupted operation even during internet outages.

### **10. On-Demand Local Python Reporting Engine**

Neo-Sentry includes a standalone Python analytics engine capable of reading exported telemetry `.csv` files and automatically generating publication-ready clinical trend reports using `matplotlib`.

---

## Usage Instructions

Deploying Neo-Sentry in a clinical environment is designed to be simple, modular, and adaptable to both connected and offline healthcare infrastructures.

### General System Initialization

1. Connect the main ESP32 control unit to power.
2. The system automatically initializes:
   - OLED telemetry display
   - Environmental sensors
   - Motion monitoring systems
   - Alarm modules
3. Once initialization completes, the OLED dashboard begins displaying live telemetry.
4. The system continuously monitors:
   - Temperature conditions
   - Patient stillness duration
   - Alarm states
   - Hardware switching conditions

If abnormal conditions are detected:
- The buzzer alarm activates
- Cooling fan or heating LED responds automatically
- Remote alerts are dispatched when network services are available

---

## Online Mode (Hospital Wi-Fi Available)

When Wi-Fi infrastructure is available, Neo-Sentry enables full cloud-connected telemetry.

### Features Available in Online Mode

- Telegram Bot Alerts
- ThingSpeak Cloud Graphing
- Google Sheets Data Logging
- Real-Time Remote Monitoring

### Wi-Fi Connection Procedure

Neo-Sentry supports both:
- Online cloud-connected monitoring
- Offline self-hosted fallback monitoring

---

### Option 1 — Hospital Wi-Fi Available (Online Mode)

Configure the hospital Wi-Fi credentials inside the firmware source code:

```cpp
// --- Wi-Fi Credentials ---
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

After firmware upload:
- The ESP32 connects to the hospital network
- Telegram alerts become active
- ThingSpeak cloud telemetry initializes
- Google Sheets live logging begins automatically

---

### Option 2 — No Wi-Fi Infrastructure Available (Offline Mode)

If no hospital Wi-Fi is available, Neo-Sentry automatically provisions a self-hosted fallback hotspot using the ESP32.

### Offline Dashboard Access Steps

1. Open Wi-Fi settings on your mobile device or computer.
2. Connect to the Neo-Sentry ESP32 hotspot.
3. Open your browser.
4. Navigate to:

```plaintext
192.168.4.1
```

### Offline Features Available

- Real-time telemetry dashboard
- Live environmental graphs
- Alarm monitoring
- Hardware activity visualization
- Local monitoring without internet dependency

This fallback architecture ensures uninterrupted neonatal monitoring even in low-resource or disconnected healthcare environments.

---

## Tech Stack

The Neo-Sentry ecosystem combines embedded intelligence, cloud telemetry, and lightweight healthcare automation infrastructure.

- **Microcontroller:** Espressif ESP32 (Dual-core XTensa LX6)
- **Framework:** C++ / Arduino Core (PlatformIO)

### Sensors
- APDS9960 (Gesture and Optical Sensor)
- BMP180 (Temperature and Pressure Sensor)
- MPU6050 (Motion and Stillness Detection)

### Actuators
- 5V DC Cooling Fan
- Red LED
- Active Hardware Buzzer

### Cloud Infrastructure
- ThingSpeak
- Google Sheets API
- Telegram Bot API

### Display
- 0.96" I2C OLED Display (SSD1306)
---

## Requirements / Installation

### Hardware Requirements

### Core Sensor & Display Connections

### Component Pin Connections

| Component | Pin Connection |
|---|---|
| Buzzer | Digital Pin D12 |
| LED | Digital Pin D18 |
| Fan (Relay Control) | Digital Pin D5 |

Relay Module & External 9V Fan Isolation Wiring
The cooling fan is driven by an external 9V battery power supply isolated via a mechanical relay module to protect the ESP32 motherboard rails.

Relay Signal Connections:
VIN (Signal Input): Connect to D5 on the ESP32.

VCC (Power): Connect to 3.3V on the ESP32.

GND (Ground): Connect to GND on the ESP32.

High-Power Load Connections:
COM (Common Terminal): Connect to the Positive (+) terminal of the 9V battery.

NO (Normally Open Terminal): Connect to the Positive (+) terminal of the Fan.

Ground Return Link: Connect the Negative (-) terminal of the Fan and the Negative (-) terminal of the 9V battery directly back to the GND rail of the ESP32 motherboard.

```plaintext
ESP32
 ├── APDS9960
 ├── OLED Display
 ├── BMP180
 └── MPU6050
```

## Software Requirements

1. Clone the repository to your local machine.

2. Open the project folder in **VS Code** with the **PlatformIO** extension installed.

3. Configure your Wi-Fi credentials inside the firmware source code:

```cpp
// --- Wi-Fi Credentials ---
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

4. Configure your Telegram Chat ID:

```cpp
#define CHAT_ID "PASTE_YOUR_CHAT_ID_HERE"
```

5. Install the required Python dependencies for the local reporting engine:

```plaintext
pip install pandas matplotlib weasyprint
```

6. Compile and upload the firmware to the ESP32:

```plaintext
pio run --target upload
```

---

## Connecting to the Neo-Sentry Telegram Bot

To receive real-time incubator alerts and monitoring updates directly on your smartphone:

### Step 1 — Open the Bot

Search for the following bot inside Telegram:

```plaintext
@Incubator_neoSentry_bot
```

### Step 2 — Activate the Bot

Press the **Start** button to allow the bot to send notifications to your account.

### Step 3 — Retrieve Your Personal Chat ID

1. Search for:

```plaintext
@userinfobot
```

2. Press **Start**.
3. Copy the numerical ID provided by the bot.

### Step 4 — Insert Chat ID into Firmware

Paste your copied Chat ID into the firmware configuration:

```cpp
#define CHAT_ID "PASTE_YOUR_COPIED_ID_HERE"
```

---

## Monitoring Live Cloud Telemetry

### ThingSpeak Dashboard

The Neo-Sentry system streams live telemetry directly to ThingSpeak cloud dashboards.

Access the live dashboard here:

```plaintext
https://thingspeak.mathworks.com/channels/3388454
```

---

## Google Sheets Historical Logging

All environmental telemetry and operational timestamps are continuously appended to a live Google Sheets database.

Access the public telemetry sheet here:

```plaintext
https://docs.google.com/spreadsheets/d/1CYqXfj2ydENCGktqIENCoc1UpGHSjCNpTQpsZO7Fu88/edit?gid=0#gid=0
```

The document is configured as read-only for public safety while still updating dynamically in real time.

4. Compile and upload the firmware to the ESP32:

```plaintext
pio run --target upload
```

---

## File Structure

The firmware is structured for modularity and easy contribution:

```plaintext
neo-sentry/
├── src/
│   ├── main.cpp                 # Core application loop
│   ├── GestureControl.cpp       # APDS-9960 driver logic
│   ├── ThermalRegulation.cpp    # PID loop for heating/cooling
│   ├── CloudTelemetry.cpp       # Wi-Fi and ntfy.sh HTTP client
├── include/
│   ├── Config.h                 # Pin definitions and global constants
│   ├── secrets.h                # Network credentials (git-ignored)
├── platformio.ini               # Build configurations and library dependencies
├── README.md                    # Project documentation
└── hardware/                    # Schematics and 3D printable STL files
```

---

## Future Roadmap

Neo-Sentry 1.0 is just the beginning. Our vision for the next iteration includes:

- **AI-Driven Predictive Analytics:** Implementing TinyML on the edge to analyze micro-fluctuations in temperature and respiration, predicting potential distress events *before* they trigger standard thresholds.
- **Battery Backup Ecosystem:** Integration of seamless Li-ion failover systems to ensure uninterrupted operation during grid power failures common in rural areas.
- **Centralized Hospital Dashboard:** A local web server running on a Raspberry Pi to aggregate data from dozens of Neo-Sentry units across an entire ward into a single React-based UI.

---

## Conclusion

We believe that cutting-edge healthcare technology shouldn't be confined to elite institutions. Neo-Sentry proves that with thoughtful engineering, open-source principles, and a relentless focus on solving real human problems, we can build a future where every premature infant has a fighting chance. 

**This is touchless intelligence. This is affordable critical care.**

---

## References

1. World Health Organization (WHO) - Guidelines on basic newborn resuscitation.
2. Espressif Systems - ESP32 Technical Reference Manual.
3. Libraries utilized: myosa.h, WiFi.h, Wire.h, WiFiClientSecure.h, UniversalTelegramBot.h, ArduinoJson.h, ThingSpeak.h, ESP_Google_Sheet_Client.h
