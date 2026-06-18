const express = require('express');
const http = require('http');
const path = require('path');
const mqtt = require('mqtt');
const socketIo = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

const PORT = process.env.PORT || 3000;

// Serve static UI assets
app.use(express.static(path.join(__dirname, '../public')));

// Connect to Public MQTT Broker (Broker acts as the IoT bridge)
const MQTT_BROKER = 'mqtt://broker.hivemq.com';
const MQTT_TOPIC = 'iot/earthquake/seismic/data';
const mqttClient = mqtt.connect(MQTT_BROKER);

mqttClient.on('connect', () => {
    console.log(`Connected to MQTT Broker at ${MQTT_BROKER}`);
    mqttClient.subscribe(MQTT_TOPIC);
});

mqttClient.on('message', (topic, message) => {
    try {
        const payload = JSON.parse(message.toString());
        // Broadcast parsed telemetry data instantly to all open web browsers
        io.emit('seismic-telemetry', payload);
    } catch (err) {
        console.error('Data parsing error:', err.message);
    }
});

server.listen(PORT, () => {
    console.log(`Web application running on http://localhost:${PORT}`);
});
