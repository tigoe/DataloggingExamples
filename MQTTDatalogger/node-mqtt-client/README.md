# MQTT Node.js  Client 

This server-side script connects to [shiftr.io](https://shiftr.io)'s public broker, reads a given topic,
and sends the result as a POST request with a JSON body
to a data storage server. Assumes it can make a HTTP POST
request something like this:

````
POST: request header: Content-Type: application/json
      request body: {"lux": 200, "ct": 5200, "id": "23EF499"}
````
There is a second client in the project that will read and parse multiple MQTT subtopics 
and form them into a single JSON string to send to the server. To use it, change the `package.json`
file as follows. Change: 
````
"start": "node server.js"
````
to
````
"start": "node server-multiple.js"
````


created 13 Jun 2021\
by Tom Igoe