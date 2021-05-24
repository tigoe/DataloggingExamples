#  DataLoggerHttpClientJSON_redirect Example

This example performs the same functionality as the [DataLoggerHttpClientJSON](../DataLoggerHttpClientJSON) example, and also processes the redirect response from the Google Script. When this sketch gets the 302 response, it reads the headers, parses the new server and API route from the `location` field, makes the second request to that address, and gets the response back. For more on this, see the [Google script guide on redirects](https://developers.google.com/apps-script/guides/content#redirects). 
.

The main difference between this sketch and [DataLoggerHttpClientJSON](../DataLoggerHttpClientJSON) is in the main loop, where they check the response from the server. The other sketch simply accepts a 200 or 302 response code as valid. This sketch looks for the 302, and then calls a new function, `redirectRequest()`, which looks through the first response's headers to get the redirect location, and makes a second HTTPS call to that location to get the final response. 
