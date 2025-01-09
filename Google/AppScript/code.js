/**
 * This is the code for the Google App Script that is used to interface with the Google Sheet.
 * 
 * true file name should be code.gs!
 */

function testPost() {
  var e = {};
  e.parameter = {};
  // example payload
  e.parameter.data =
    `
  {
    "Temp": "24.4",
    "humidity": "44.4",
    "dewPt": "15",
    "hi": "32.5"
  }
  `
  e.parameter.coreid = 'deh debugger';
  e.parameter.published_at = new Date().toISOString();
  doPost(e);
}

// Post Function
function doPost(e) {
  /**
   * Data format:
   * 
   * e.parameter.data
   * e.parameter.coreid
   * e.parameter.published_at "2016-04-16T13:37:08.728Z"
   */
  // 

  // for verbosing to Particle side
  var result = {};

  // convert the date to a date object (formatting)
  var publishedAt = new Date(e.parameter.published_at);

  // array to be place in sheet
  // (sheets take in an array)
  let dataArray = [];

  try {
    let _doc = JSON.parse(e.parameter.data);
    for (blah in _doc) {
      dataArray.push(_doc[blah]) // blah is a key
    }
    
    var sheet = SpreadsheetApp.getActiveSheet();

    var row = [e.parameter.coreid, publishedAt];

    // put the data altogether in an array
    row = row.concat(dataArray);

    sheet.appendRow(row);

    result = { status: 'success'};
  }
  catch (e) {
    console.error("Error in parsing");
    result = { status: 'failed', message: error.toString() };
  }

  return ContentService.createTextOutput(JSON.stringify(result))
    .setMimeType(ContentService.MimeType.JSON);
}

// debug with dummy data
function testGet() {
  var e = {};
  e.parameter = JSON.parse(
    // :: data with every parameter ::
    // `{
    //   // "beginDate": "Fri Jan 03 2025 00:00:00 GMT-0600 (Central Standard Time)",
    //   // "endDate": "Sat Jan 10 2025 00:00:00 GMT-0600 (Central Standard Time)",
    //   "limit": 69
    // }  `);

    // :: data with limit only ::
    `{
    "limit": 69
  }  `);
  doGet(e)
}

// // get func
function doGet(e) {
  let toR; // JSON that will be returned
  let result; // the actual results that are put into toR later on (sigh....)

  try {
    let sheet = SpreadsheetApp.getActiveSheet();

    let data = e.parameter; // save it for now

    /**
     * Settings for stuff to return
     * 
     * >> if no date settings, just return the most recent data regarding the limit
     * 
     * >> else adhere to normal settings
     * 
     * if limit is null as well, we can instantly return an error.
     */
    let start, end, limit = null;



    // set parameters  
    if (data["beginDate"]) {
      start = new Date(data["beginDate"]);
    }
    if (data["endDate"]) {
      end = new Date(data["endDate"]);
    }
    limit = data["limit"];


    //check for parameters
    if (limit) {
      // search with date in mind
      // note [] check if dates are valid (?)
      if (start && end) {
        result = getData(limit, start, end);
      }
      else {
        result = getData(limit);
      }
    }
    else {
      throw new Error("No limit specified");
    }

    // console.log(result);
    toR = { status: 'success', data: result };
  } catch (error) {
    toR = { status: 'failed', message: error.toString() };
    // console.log(toR);
  }
  console.log(toR)
  return ContentService.createTextOutput(JSON.stringify(toR)).setMimeType(ContentService.MimeType.JSON);
}

// where the header is in sheet
const __HeaderIndex__ = 1
// where the date is in row in sheet
const __DateIndex__ = 1

/**
 * Linearly search for the data in a sheet in a Google Spreadsheet
 * 
 * @Param: Start and end should be date.
 */
function getData(limit, start = null, end = null) {
  var sheet = SpreadsheetApp.getActiveSheet();
  var data = sheet.getDataRange().getValues();

  let toR = []; // (to Return)

  var headers = data[1];

  //row
  // start from the end, and go forwards until the end date is found
  // and then record until limit or begin date
  for (var i = data.length - 1; i > __HeaderIndex__; i--) {
    var row = data[i];
    var rowData = {};

    if (toR.length >= limit) {
      return toR;
    }
    // if date is specified, then actually look for date
    // else just get the last few
    if (start && end) {

      // check the date of the row
      let rowDate = new Date(row[__DateIndex__]);
      if (rowDate < end && rowDate > start) {
        // append the data into toR as JSON
        // (sheet rows = array, hence we have to manually convert)
        for (var j = 0; j < headers.length; j++) {
          rowData[headers[j]] = row[j]; // set rowData entry using headers.
        }
        // store it!
        toR.push(rowData);
      }
    }
    else {
      // set columns as JSON
      for (var j = 0; j < headers.length; j++) {
        rowData[headers[j]] = row[j]; // set rowData entry using headers.
      }
      // store it!
      toR.push(rowData);
    }
  }
  return toR;
}