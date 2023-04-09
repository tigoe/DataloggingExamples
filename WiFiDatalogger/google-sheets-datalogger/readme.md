
# Google Sheets Datalogger

![system diagram of a datalogger connected to a Google spreadsheet, as described below.](../../images/wifi-datalogger-google-sheets.png)

_Figure 2. System diagram of the Google apps script datalogger_

The [Google Apps script](https://developers.google.com/apps-script/reference/spreadsheet/spreadsheet-app) in this repository allows you to save data to a Google Sheets spreadsheet via HTTP. Arnab Chakravarty has a [tutorial on how it works](https://github.com/AbolTaabol/Arduino-GoogleSheet_Logger). For more background on Google Apps scripts, see the link above, or [this link](https://developers.google.com/apps-script/guides/web) which explains the web functions `doGet()` and `doPost()`, which are the main functions of this example. 

The structure of the system is similar to the node server, and is diagrammed in Figure 2. The microcontroller only has to change the URL and route that it's sending data to in order to send data to the Google apps script. The script writes to a Google Sheets spreadsheet that you need to create.

What's great about the Google Apps scripting API is that it's just JavaScript, it lets you give your spreadsheet into a web service quickly,   and it gives you everything you might need to know about a given HTTP request in JSON, so it's easy to parse it out and put it into the cells of a spreadsheet. This lets you take advantage of all the things you can do with the data in a spreadsheet: sorting, graphing, filtering, and so forth.

Briefly, the steps to get started are as follows:

1. Set up a spreadsheet
2. Give the sheet some column headers
3. Publish it to the web as a web page using the File -> Publish for Web menu
4. Set up a script using the Tools -> Script Editor menu
5. Write the script
6. Deploy it
7. Note the relevant URLs and modify the Arduino client to contact them. 

The [google-sheets-datalogger](https://github.com/tigoe/DataloggingExamples/tree/main/WiFiDatalogger/google-sheets-datalogger) script in this collection will accept both GET and POST requests. For the POST request, the data format is the same as the node.js server above:

````js
{ "uid": client ID, a 9-byte ID in hexadecimal format,
  "date": date in ISO8601 format}
````
As with the node.js server, you can also include any sensor characteristics that you want to add. The Arduino examples in this collection send light and color temperature levels in lux (`lux`) and degrees Kelvin (`ct`), respectively, and the script looks for those characteristics as names of the column headings in the spreadsheet. 

This script works a bit differently from the node.js server above. The names of your client's sensor characteristics must match the column headings in the spreadsheet.  When a client sends a request, the script reads the names of the column headings in the spreadsheet and compares the characteristics' key names from the JSON object with the column headings. If the client's data has a characteristic matching one of the column heads, the script inserts the data into that column. For example, the Arduino client in this repository sends the following: `uid`,`dateTime`,`lux`,`ct`,`uptime`. So the Google sheet needs headings with the same names in order to capture it all. 


#### Google Script URLs

To connect to the Google script from the Arduino clients, you need to change the server address and the API route. The server address is always `script.google.com`. This is not the address of your actual spreadsheet, it's the address of the spreadsheet's script. 

The API route will depend on your script's deployment. The URL will change every time you deploy the script. To get it, click the Deploy button in the script editor, choose "Manage Deployments, and copy the Web URL of the current deployment. It will look something like this:

````
https://script.google.com/macros/s/XXXXXX/exec
````
You can see the server address mentioned above in this URL. The API route is the part after the server; in this case, it's `macros/s/XXXXX/exec`. It will be different for your script. Copy it, and paste it into the `route` global variable in the [DataLoggerHttpClientJSON](https://github.com/tigoe/DataloggingExamples/tree/main/WiFiDatalogger/DataLoggerHttpClientJSON) or [DataLoggerHttpClientJSON_redirect](https://github.com/tigoe/DataloggingExamples/tree/main/WiFiDatalogger/DataLoggerHttpClientJSON_redirect) sketch. Now your Arduino should be able to connect to the script.

#### Google Script Redirects

For security, Google scripts redirect to a one-time URL, generated with each new request, to respond to requests. Your HTTPS request will get a 302 response like this:

````sh
HTTP/2 302 
content-type: text/html; charset=UTF-8
access-control-allow-origin: *
cache-control: no-cache, no-store, max-age=0, must-revalidate
pragma: no-cache
expires: Mon, 01 Jan 1990 00:00:00 GMT
date: Mon, 24 May 2021 15:39:34 GMT
location: https://script.googleusercontent.com/macros/echo?user_content_key=XXXXXXXXXXXXXXXXXXXXX
````
You can see that one of the headers of the response is the location of this URL. When you get that, you need to make your request again, using the server and route in the redirect location. 

The script will still add your data to the sheet if it's valid, even when it gives you a 302 response. So you can use the [DataLoggerHttpClientJSON](https://github.com/tigoe/DataloggingExamples/tree/main/WiFiDatalogger/DataLoggerHttpClientJSON) and accept the 302 response if you want. The sketch will still work with the script, even if it's not reading the whole response. 

If you want the full response from the server, however, try the [DataLoggerHttpClientJSON_redirect](https://github.com/tigoe/DataloggingExamples/tree/main/WiFiDatalogger/DataLoggerHttpClientJSON_redirect) sketch. When this sketch gets the 302 response, it reads the headers, parses the new server and API route from the `location` field, makes the second request to that address, and gets the response back. 

For more on this, see the [Google script guide on redirects](https://developers.google.com/apps-script/guides/content#redirects). 




