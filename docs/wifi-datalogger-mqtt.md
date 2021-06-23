# Datalogging to a Server via MQTT and WiFi

The examples described here log data to a server via WiFi using a network-enabled microcontroller as a datalogging client. They are based on the [WiFi datalogging collection](wifi-datalogger), but instead of HTTP, the microcontroller clients use the [Message Queueing Telemetry Transfer (MQTT)](https://tigoe.github.io/mqtt-examples/) protocol.

These examples should all work on the network-enabled MKR boards, along with the Nano 33 IoT and other SAMD-based boards which have network capability. They can likely work on Espressif-based boards (ESP32, ESP8266) with some modification.

## What is MQTT? 

MQTT, is a lightweight IP-based messaging protocol designed for communication between sensors, controllers, and other devices. It’s designed to support equipment that may not always be online, or that have limited processing power. Unlike HTTP, it's message-based, so once a client sends a message, there's no need to wait for a response. MQTT server programs are called **brokers**. A broker keeps track of messages from clients. Clients can **publish** messages to a broker on a given topic, or can **subscribe** to an existing topic to see messages from other clients. 

## MQTT Datalogger System Diagram

Figure 1 shows the system diagram for this datalogger. There are several microcontroller-based sensor clients, shown at the top of the diagram that all publish messages to a topic called `light-readings` on an MQTT broker. There are two other clients: a browser-based client written in HTML and JavaScript, and a server-based client written in node.js. The HTML/JS client subscribes to the `light-readings` topic and displays any incoming messages in the browser. The node.js client also subscribes to the `light-readings` topic. When it receives a new message, it makes a HTTP POST request to a Google Script server that posts to a Google Sheets spreadsheet. This Google script/spreadsheet combination is the same as what's used in the [Google Sheets Datalogger example](https://tigoe.github.io/DataloggingExamples/wifi-datalogger#google-sheets-datalogger). 

The advantage of using MQTT for this datalogging application is that the microcontrollers don't have to wait for a reply from the server, and can therefore be inactive and using less energy more of the time. It's also simpler to program than an HTTP client.

![Figure 1. System diagram of the MQTT datalogger](images/mqtt-to-http-datalogger.png)

## MQTT Clients

There are several MQTT microcontroller clients in this repository:
* DataLoggerMqttJSON - the simplest of the clients. Reads a sensor and sends an MQTT message as a JSON object. 
* DataLoggerMqttJSONRTC - similar to the first client, but uses the alarm function of the RTCZero library to control when messages are sent. 
* DataLoggerMqttJSONWithStatusCheck - Based on the first client, and also listens on a UDP port for specific commands that let you check the status of the controller or restart it.
* DataLoggerMqttSubTopics - sends the sensor properties as individual values that are subtopics of the `light-readings` topic rather than as a single JSON object. 

