# 📡 IoT Earthquake Monitoring Web Application

An end-to-end IoT solution that monitors structural vibrations and seismic data in real-time. This system uses an **ESP32** microcontroller coupled with an **MPU6050** accelerometer to capture telemetry, routes it via an cloud **MQTT Broker**, and streams it to a web-based dashboard using **WebSockets** for live seismograph visualizations and instantaneous alert triggering.

---

## 🏗️ System Architecture

1. **Hardware Node (Edge):** The ESP32 continuously polls the MPU6050 sensor to calculate 3D acceleration vectors ($g$-force forces). 
2. **IoT Data Bridge (Ingestion):** Telemetry is wrapped into lightweight JSON payloads and published to an MQTT Broker over Wi-Fi.
3. **Application Server (Backend):** A Node.js engine subscribes to the MQTT topic, processes the stream, and distributes it instantly to active clients via Socket.io.
4. **Live Dashboard (Frontend):** A responsive web UI rendering real-time scrolling seismographs using Chart.js, flashing visual alarms during critical structural alerts.

---

## 📂 Repository Layout

```text
├── hardware/
│   └── esp32_seismic_node.ino   # Arduino core ESP32 sensor firmware
├── server/
│   ├── package.json             # Node.js backend environment configurations
│   └── server.js                # Express app & MQTT-WebSocket broker relay
└── public/
    └── index.html               # Frontend dashboard UI with real-time Chart.js



