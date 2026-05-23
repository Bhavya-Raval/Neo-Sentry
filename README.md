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

### **1. Aseptic Gesture-Based Touchless Control**

In a NICU, every physical touch is a potential vector for pathogens. Neo-Sentry eliminates the need for physical buttons or touchscreens. Using optical proximity sensing arrays, medical staff can navigate menus, adjust temperature targets, and silence alarms purely through calibrated hand gestures.
- **Swipe Right/Left:** Navigate telemetry pages.
- **Hold (Hover):** Confirm selection or toggle power states.

### **2. Real-Time Atmospheric Stabilization**

Premature infants cannot regulate their own body temperature. Neo-Sentry employs high-precision thermal and humidity sensors continuously feeding data into an aggressive PID control loop.
- The system dynamically modulates heating elements and ventilation fans to maintain the micro-environment within a strict **0.5°C tolerance** of the target parameter.
- Contactless respiration monitoring observes chest displacement to infer respiratory rates without distressing the infant with adhesive sensors.

### **3. Cloud Telemetry & Asynchronous Alerts**

Awareness is critical. Neo-Sentry doesn't just display data locally; it pushes critical states to the cloud.
- Utilizing **ntfy.sh**, the system broadcasts encrypted, real-time alerts to the smartphones of attending physicians.
- From critical temperature deviations to power failure warnings, the care team is instantly notified, no matter where they are in the hospital.

### **4. Extreme Affordability Paradigm**

By utilizing off-the-shelf, mass-produced IoT components and open-source software, the total bill of materials (BOM) is drastically reduced. We replace bespoke, expensive medical-grade controllers with the versatile ESP32, achieving comparable reliability through redundant software architecture.

---

## Usage Instructions

Deploying Neo-Sentry in a clinical environment is designed to be frictionless.

1. **Power Initialization:** Connect the main power supply (12V DC input). The system will boot and perform a self-diagnostic sequence verifying all sensor baselines.
2. **Network Handshake:** The ESP32 will attempt to connect to the pre-configured hospital Wi-Fi network. The onboard OLED will display `[ SYS O-LINE ]` upon success.
3. **Parameter Configuration:** Use a sweeping right-hand gesture over the optical sensor to enter the setup menu. Hover to select the target temperature (default: 36.5°C).
4. **Monitoring:** Once the infant is secured, the system operates autonomously. Subscribe to the designated `ntfy.sh` topic on your mobile device to receive alerts.

To view the raw serial telemetry during debugging, use the following command via your local terminal:

```plaintext
pio device monitor -b 115200
```

---

## Tech Stack

The Neo-Sentry ecosystem is a symphony of modern embedded hardware and agile cloud services:

- **Microcontroller:** Espressif ESP32 (Dual-core XTensa LX6)
- **Framework:** C++ / Arduino Core (PlatformIO)
- **Sensors:**
  - APDS-9960 (RGB and Gesture Sensor)
  - DHT22 / SHT31 (High-Precision Temperature & Humidity)
  - VL53L0X (Time-of-Flight for Respiration Inference)
- **Actuators:** 12V PTC Heating Elements, PWM DC Ventilation Fans
- **Cloud Infrastructure:** ntfy.sh (HTTP POST based pub-sub notification service)
- **Display:** 0.96" I2C OLED (SSD1306)

---

## Requirements / Installation

### Hardware Requirements
- Custom Neo-Sentry PCB or standard breadboard wiring.
- Components listed in the Tech Stack.

### Software Installation

1. Clone the repository to your local machine.
2. Open the project folder in **VS Code** with the **PlatformIO** extension installed.
3. Rename `secrets.example.h` to `secrets.h` and insert your local Wi-Fi credentials and target `ntfy.sh` topic.

```cpp
// secrets.h
#define WIFI_SSID "Hospital_Secure_Network"
#define WIFI_PASS "securepassword123"
#define NTFY_TOPIC "neosentry_ward_A"
```

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
3. ntfy.sh Official Documentation - Push notifications made easy.
4. Open-source libraries utilized: Adafruit Unified Sensor, ArduinoJson.
