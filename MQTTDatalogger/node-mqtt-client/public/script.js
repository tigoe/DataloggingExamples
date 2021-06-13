/*
  Hue Hub light control over MQTT
  
  created 23 July 2020
  modified 23 Nov 2020
  by Tom Igoe
*/

// MQTT broker details:
let broker = {
   hostname: 'public.cloud.shiftr.io',
   port: 443
};

// MQTT client:
let client;
// client credentials:
let creds = {
   clientID: 'myClient',
   userName: 'public',
   password: 'public'
}
// topic to subscribe to when you connect to the broker:
let topic = 'lights';


function setup() {

   // Create an MQTT client:
   client = new Paho.MQTT.Client(broker.hostname, Number(broker.port), creds.clientID);
   // set callback handlers for the client:
   client.onConnectionLost = onConnectionLost;
   client.onMessageArrived = onMessageArrived;
   // connect to the MQTT broker:
   client.connect(
       {
           onSuccess: onConnect,       // callback function for when you connect
           userName: creds.userName,   // username
           password: creds.password,   // password
           useSSL: true                // use SSL
       }
   );
}


// called when the client connects
function onConnect() {
   localDiv.html('client is connected');
   client.subscribe(topic);
}

// called when the client loses its connection
function onConnectionLost(response) {
   if (response.errorCode !== 0) {
       localDiv.html('onConnectionLost:' + response.errorMessage);
   }
}

// called when a message arrives
function onMessageArrived(message) {
   remoteDiv.html('I got a message:' + message.payloadString);
   let  incomingNumber = parseInt(message.payloadString);
}
