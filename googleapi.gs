function doGet(e) {
  const spreadsheetId = '15ODUqaZt0XR8L83GgnU2j2FXJ6Nciul-Q-PU3takrYw';
  // const met = e.parameter.met;
  met = 0;
  if(met == 0){
    try {
      const values = Sheets.Spreadsheets.Values.get(spreadsheetId, 'StudentID!A2:B1000').values;
      if (!values) {
        // No data found
        return ContentService.createTextOutput("ERROR");
      }
      var outputString = "[";
      for(var i = 0; i < values.length; i++){
        if(values[i][1] != undefined){
          outputString += '"'+values[i][0].toString().replace(/[\[\]\|\\\",]/g, '')+'|||'+values[i][1].toString().replace(/[\[\]\|\\\",]/g, '')+'", ';
        }
      }
      outputString += '""]';
      return ContentService.createTextOutput(outputString);
    } catch (err) {
      console.log(err.message);
    }
  }else if(met == 1){
    try {
      const values = Sheets.Spreadsheets.Values.get(spreadsheetId, "Log!F1").values;
      if (!values) {
        // No data found
        return ContentService.createTextOutput("ERROR");
      }
      try {
        var ss = SpreadsheetApp.openById(spreadsheetId);
        var sheet = ss.getSheetByName('Log');
        const range = sheet.getRange(Number(values[0][0])+1, 1, 1, 5);
        const inout = e.parameter.inout;
        const name = e.parameter.name;
        const reason = e.parameter.reason;
        var date = new Date(); 
        range.setValues([[inout, Utilities.formatDate(date, "Pacific/Auckland", "d/M/y"), Utilities.formatDate(date, "Pacific/Auckland", "h:mm:ss ")+(date.getHours() >= 12 ? 'PM' : 'AM'), name, reason]]);
        SpreadsheetApp.flush();
        return ContentService.createTextOutput("Success");
      } catch (err) {
        console.log('Failed with error %s', err.message);
      }
    } catch (err) {
      console.log(err.message);
    }
  }else{
    try {
      const values = Sheets.Spreadsheets.Values.get(spreadsheetId, "StudentID!C1").values;
      if (!values) {
        // No data found
        return ContentService.createTextOutput("ERROR");
      }
      try {
        var ss = SpreadsheetApp.openById(spreadsheetId);
        var sheet = ss.getSheetByName('StudentID');
        const range = sheet.getRange(Number(values[0][0])+1, 1, 1, 2);
        var date = new Date(); 
        range.setValues([[e.parameter.name, e.parameter.data]]);
        SpreadsheetApp.flush();
        return ContentService.createTextOutput("Success");
      } catch (err) {
        console.log('Failed with error %s', err.message);
      }
    } catch (err) {
      console.log(err.message);
    }
  }
  return ContentService.createTextOutput("ERROR");
}

function clear(){
  const spreadsheetId = '15ODUqaZt0XR8L83GgnU2j2FXJ6Nciul-Q-PU3takrYw';
  try {
    var ss = SpreadsheetApp.openById(spreadsheetId);
    var sheet = ss.getSheetByName('Log');
    const range = sheet.getRange('A2:D10000');
    range.clear();
    SpreadsheetApp.flush();
    return ContentService.createTextOutput("Success");
  } catch (err) {
    console.log('Failed with error %s', err.message);
  }
  return ContentService.createTextOutput("ERROR");
}
