/*
  MQTT Client
  
  created 13 Jun 2021
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
   clientID: 'htmlClient',
   userName: 'public',
   password: 'public'
}
// topic to subscribe to when you connect to the broker:
let topic = 'light-readings';
let localDiv, remoteDiv;

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
   // add handles for the divs:
   localDiv = document.getElementById('local');
   remoteDiv = document.getElementById('remote');
}


// called when the client connects
function onConnect() {
   localDiv.innerHTML = 'client is connected to '
      + broker.hostname + '<br>and subscribed to '
      + topic;
   client.subscribe(topic);
}

// called when the client loses its connection
function onConnectionLost(response) {
   if (response.errorCode !== 0) {
      localDiv.innerHTML = 'Connection Lost:' + response.errorMessage;
   }
}

// called when a message arrives
function onMessageArrived(message) {
   // get the payload string and make it a JSON object:
   let msg = JSON.parse(message.payloadString);
// iterate over the elements of the JSON object:
   for (var key in msg) {
      // If there is not an HTML element with the same ID:
      if (document.getElementById(key) === null) {
         // create create one and give it this ID:
         let thisDiv = document.createElement('div');
         thisDiv.id = key;
         // create text with the key and value:
         let textNode = document.createTextNode(key + ': ' + msg[key]);
         // add the text to the element and add it to the HTML:
         thisDiv.append(textNode);
         document.body.append(thisDiv);
      } else {
         // if there's already an element with this ID,
         // just update it:
         let thisDiv = document.getElementById(key);
         thisDiv.innerHTML = key + ': ' + msg[key];
      }
   }
}

// This is a listener for the page to load.
// This is the command that actually starts the script:
window.addEventListener('DOMContentLoaded', setup);
