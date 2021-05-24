/*
  Datalogger script to support GET and POST requests to insert into a Google Sheets spreadsheet. 

  This version accepts either GET or POST requests, and parses the query string (if a GET request)
  or the request body (if a POST request) looking for keys corresponding to the values in row 1 of the
  spreadsheet. If any of the keys in the request that matches a column header, then the corresponding 
  values are added to a new row in the sheet. Column header names are case sensitive.

  For example, if your header rows are: id, datetime,lux,ct, and local_timestamp, 
  then your requests would look like this:
  GET: https://script.url?ct=5200&lux=200&id=23EF499
  POST: request header: Content-Type: application/json
        request body: {"lux": 200, "ct": 5200, "id": "23EF499"}

  The script filters for known clients. It checks for a key called uid in the request data, 
  and that the value associated with that key is in the knownClients array. If both are not true, 
  the data is not posted.

  The script also appends a timestamp in a column called local_timestamp so you can check if the client's
  timestamps are in sync with the google drive server. 

 
  Adapted from Arnab Chakravarty's script at https://github.com/AbolTaabol/Arduino-GoogleSheet_Logger
  created 22 May 2021
  by Tom Igoe

*/

// constants:
// URL of the Google Sheet (for viewing, and for the script to write to):
const SHEET_URL = 'https://docs.google.com/spreadsheets/XXXX';
// URL of this script (the client will make calls to this):
const SCRIPT_URL = 'https://script.google.com/macros/XXX';
// column for a local timestamp:
const TIMESTAMP = 'local_timestamp';

// Error codes for return result:
const NO_MATCHING_PARAMS = -1;    // client didn't send matching params
const NO_PARAMS_SENT = -2;        // client didn't send any parameters
const WRONG_CONTENT_TYPE = -3;    // request must be content-type: application/json
const UNKNOWN_CLIENT = -4;        // client UID not in list
const SUCCESS = 0;              // all good, data saved

// uids of known clients:
let knownClients = ["AA00BB11CC22DD33EE","AA11BB22CC33DD44EE" ];

//Get the document:
var ss = SpreadsheetApp.openByUrl(SHEET_URL);
// get the sheets in the document:
var sheets = ss.getSheets();
// get the first sheet:
var sheet = sheets[0];

// get the last row and column number:
var lastRow = sheet.getLastRow();
var lastCol = sheet.getLastColumn();

//GET request
function doGet(e) {
  // default response
  var response;
  // get the parameters from the query string. see 
  // https://developers.google.com/apps-script/guides/web for more info:
  var results = e.parameter;
  // make sure the parameter string is not empty:
  if (Object.keys(results).length > 0) {
    response = insertData(results);
  } else {
    response = NO_PARAMS_SENT;
  }

  // return a result to the client:
  return ContentService.createTextOutput(response);
}

//POST request
function doPost(e) {
  // default response
  var response = false;
  // check to see that the body is the right content type:
  if (e.postData.type === 'application/json') {
    // get the body, convert to JSON:
    var results = JSON.parse(e.postData.contents);
    // make sure the request body is not empty:
    if (Object.keys(results).length > 0) {
      // insert into the sheet:
      response = insertData(results);
    } else {
      response = NO_PARAMS_SENT;
    }
  } else {
    response = WRONG_CONTENT_TYPE;
  }
  // return a result to the client:
  return ContentService.createTextOutput(response);
}

/* 
  Assumes a JSON object, data, and creates a new row at the end
  of the spreadsheet if any of the header row names correspond 
  to the keys in data. Order of the keys in data do not matter
*/
function insertData(requestData) {
  var result = 0;
  // set up a new array for the data that you want to insert:
  var insertValues = new Array();
  // get the column names in the header row. 
  // these should correspond to the key values in the data sent:
  var colNames = sheet.getRange(1, 1, 1, lastCol).getValues()[0];

  //iterate over the data object:
  for (item in requestData) {
    // check to see if this is a known client, and if not, stop:
    if (item === 'uid' && !knownClients.includes(requestData[item])) {
      return UNKNOWN_CLIENT;
    }
    // get the position header row whose value corresponds to this item's key:
    var thisColumn = colNames.indexOf(item);
    // if the key's in the header row, insert the value
    // into the insertValues array:
    if (thisColumn > -1) {
      insertValues[thisColumn] = requestData[item];
    }
  }


  // now you have an array with the values in the right order.
  // if the array length > 0, insert it at the end of the sheet:
  if (insertValues.length > 0) {
    // get the position of the column for the local timestamp:
    var timeStampColumn = colNames.indexOf(TIMESTAMP);
    // only do it if there is a timestamp column:
    if (timeStampColumn > -1) {
      insertValues[timeStampColumn] = new Date().toISOString();
    }
    //add new row to spreadsheet at the end:
    sheet.appendRow(insertValues);
    result = SUCCESS;
  } else {
    result = NO_MATCHING_PARAMS;
  }

  return result;
}