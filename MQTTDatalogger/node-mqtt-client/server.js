/*
   MQTT Node.js  client. 
  Connects to shiftr.io's public broker, reads a given topic,
  and sends the result as a POST request with a JSON body
  to a data storage server. Assumes it can make a HTTP POST
  request something like this:
  
  POST: request header: Content-Type: application/json
        request body: {"lux": 200, "ct": 5200, "id": "23EF499"}
  
  Connects to a google apps script at the moment, to fill in a spreadsheet.

  created 13 Jun 2021
  by Tom Igoe
*/

// include the MQTT library:
const mqtt = require("mqtt");
const fetchUrl = require("fetch").fetchUrl;

// the broker you plan to connect to.
// transport options:
// 'mqtt', 'mqtts', 'tcp', 'tls', 'ws', or 'wss':
const broker = "mqtts://public.cloud.shiftr.io";
// the URL where your data storage server is:
const storageURL =  "https://script.google.com/macros/s/XXXX/exec";
// client options:
const options = {
  clientId: "nodeClient",
  username: "public",
  password: "public"
};
// topic and message payload:
let myTopic = "light-readings";

// settings for the HTTP request to the storage server:
let httpOptions = {
  method: "POST",
  headers: {
    "Content-type": "application/json"
  }
  // google apps script expects you to redirect AND change to GET
  // from POST. That's hard to trap for, but you don't need the
  // redirect response if you get a 302. You can just ignore it
  //disableRedirects: true
};

// connect handler:
function setupClient() {
  // read all the subtopics:
  client.subscribe(myTopic);
  // set a handler for when new messages arrive:
  client.on("message", readMqttMessage);
}

// new message handler:
function readMqttMessage(topic, message) {
  // message is a Buffer, so convert to a string:
  let msgString = message.toString();
  console.log(msgString);
  // send it:
  httpOptions.payload = msgString;
  fetchUrl(storageURL, httpOptions, storageResponse);
}

function storageResponse(error, headings, body) {
  // print the responses from the server, if you need them:
  // console.log(headings);
  // console.log(body.toString());
}

// make a client and connect:
let client = mqtt.connect(broker, options);
client.on("connect", setupClient);
