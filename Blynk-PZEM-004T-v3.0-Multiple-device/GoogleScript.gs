// Created by Sujay S. Phadke, 2017
// All Rights Reserved.
// Github: https://github.com/electronicsguy
//
// Read/Write to Google Sheets using REST API.
// Can be used with ESP8266 & other embedded IoT devices.
// 
// Use this file with the ESP8266 library HTTPSRedirect
// 
// doGet() and doPost() need the spreadsheet ID. Cannot use "active spreadsheet" here since
// the device can operate without the spreadsheet even being open.
// http://stackoverflow.com/questions/4024271/rest-api-best-practices-where-to-put-parameters
// http://trevorfox.com/2015/03/rest-api-with-google-apps-script

// Similar API docs:
// https://gspread.readthedocs.org/en/latest/
// https://smartsheet-platform.github.io/api-docs/#versioning-and-changes
// http://search.cpan.org/~jmcnamara/Excel-Writer-XLSX/lib/Excel/Writer/XLSX.pm

// http://forum.espruino.com/conversations/269510/
// http://stackoverflow.com/questions/34691425/difference-between-getvalue-and-getdisplayvalue-on-google-app-script
// http://ramblings.mcpher.com/Home/excelquirks/gooscript/optimize

// Things to remember with getValue() object format:
// 1. Partial dates or times-only will be replaced with a full date + time, probably in the
//    year 1989. Like this: Sat Dec 30 1899 08:09:00 GMT-0500 (EST)
// 2. Dollar ($) currency symbol will be absent if cell contains currency.
//    This may be locale-dependent.
// 3. Scientific notation will be replaced by decimal numbers like this: 0.0000055

// Script examples
// https://developers.google.com/adwords/scripts/docs/examples/spreadsheetapp
                                  
var SS = SpreadsheetApp.openById('127JfdRicbxPlgAmPxDI0lvuexN9WoMYgZqsncqMNp_E'); // PZEMx4 googlespreadsheet
var raw        = SS.getSheetByName('RAW'); 
var sheetSetup = SS.getSheetByName('Setup'); 

var str = "";

function test() {
  var str = doPost("arguments row");
}


function doPost(e) {

  var parsedData;
  var result = {};

  var nowStamp = Utilities.formatDate(new Date(), "Europe/Kiev", "yyyy-MM-dd hh:mm:ss a");
  
  var contents = e.postData.contents;
  
  //Update actual data on RAW sheet
  
  if(raw.getMaxRows()>106){
    raw.deleteRow(106);
  }
    
  raw.insertRows(2);
  SS.getRange("RAW!A2").setValue(nowStamp);
  SS.getRange("RAW!B2").setValue(contents);
    
  
  try { 
    parsedData = JSON.parse(contents);
  } 
  catch(f){
    return ContentService.createTextOutput("Error in parsing request body: " + f.message);
  }
   
  if (parsedData !== undefined){
    // Common items first
    var flag = parsedData.format;
    
    if (flag === undefined){
      flag = 0;
    }
    
    switch (parsedData.command) {
      case "appendRow":
        var now = Utilities.formatDate(new Date(), "Europe/Kiev", "yyyy-MM-dd HH:mm:ss");
        var values = [[{}]];
        var SheetName = "";
        
        SheetName = "DATA" + parsedData.sensordatavalues.ID;
        var tmp = SS.getSheetByName(SheetName);

        if(tmp.getMaxRows()>100006){
          tmp.deleteRow(100006);
        }
        tmp.insertRows(5);
          
        //Receieved timestamp
        tmp.getRange(5, 2).setValue(now);
        values[0] = parsedData.sensordatavalues.VOLT.split(":");
        tmp.getRange("C5:E5").setValues(values);
        values[0] = parsedData.sensordatavalues.CURR.split(":");
        tmp.getRange("F5:H5").setValues(values);        
        values[0] = parsedData.sensordatavalues.POWR.split(":");
        tmp.getRange("I5:K5").setValues(values);        
        values[0] = parsedData.sensordatavalues.ENRG.split(":");
        tmp.getRange("L5").setValue(values[0][2]);
        values[0] = parsedData.sensordatavalues.FREQ.split(":");
        tmp.getRange("M5:O5").setValues(values);        
        values[0] = parsedData.sensordatavalues.POWF.split(":");
        tmp.getRange("P5").setValue(values[0][1]);   
        tmp.getRange("Q5").setValue(parsedData.sensordatavalues.MCNT);   
        
        str = "Return success";
        SpreadsheetApp.flush();          
          
    }
    
    return ContentService.createTextOutput(str);
  }
  
  else{
    return ContentService.createTextOutput("Error! Request body empty or in incorrect format.");
  }
}


function doGet(e){
  
  var val = e.parameter.value;
  var read = e.parameter.read;
  
  var nowStamp = Utilities.formatDate(new Date(), "Europe/Kiev", "yyyy-MM-dd hh:mm:ss a");
  
  //Update actual data on RAW sheet
  
  if(raw.getMaxRows()>106){
    raw.deleteRow(106);
  }
    
  raw.insertRows(2);
  SS.getRange("RAW!A2").setValue(nowStamp);
  SS.getRange("RAW!B2").setValue(e.queryString);
  
  if (read !== undefined){    
    return ContentService.createTextOutput(sheetSetup.getRange(read).getValue());
  }
  
  if (e.parameter.value === undefined)
    return ContentService.createTextOutput("No value passed as argument to script Url.");
    
}

