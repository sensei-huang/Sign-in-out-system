function doGet(e) {
  const spreadsheetId = '15ODUqaZt0XR8L83GgnU2j2FXJ6Nciul-Q-PU3takrYw';
  if(e.parameter.met == 0){
    try {
      const rangeName = 'Log!A1:D20';
      const values = Sheets.Spreadsheets.Values.get(spreadsheetId, rangeName).values;
      if (!values) {
        // No data found
        return ContentService.createTextOutput("ERROR").setMimeType(ContentService.MimeType.TEXT);
      }
      return ContentService.createTextOutput(values[0][0]).setMimeType(ContentService.MimeType.TEXT);
    } catch (err) {
      console.log(err.message);
    }
  }else{
    try {
      const rangeName = 'Log!E1';
      const values = Sheets.Spreadsheets.Values.get(spreadsheetId, rangeName).values;
      if (!values) {
        // No data found
        return ContentService.createTextOutput("ERROR").setMimeType(ContentService.MimeType.TEXT);
      }
      try {
        var ss = SpreadsheetApp.openById(spreadsheetId);
        var sheet = ss.getSheetByName('Log');
        const range = sheet.getRange(String(Number(values[0][0])+1),1, 1, 4);
        var date = new Date(); 
        range.setValues([[Utilities.formatDate(date, "Pacific/Auckland", "dd/MM/yyyy"), Utilities.formatDate(date, "Pacific/Auckland", "h:mm:ss ")+(date.getHours() >= 12 ? 'PM' : 'AM'), e.parameter.name, e.parameter.reason]]);
        SpreadsheetApp.flush();Utilities.fo
        return ContentService.createTextOutput("Success").setMimeType(ContentService.MimeType.TEXT);
      } catch (err) {
        console.log('Failed with error %s', err.message);
      }
    } catch (err) {
      console.log(err.message);
    }
  }
  return ContentService.createTextOutput("ERROR").setMimeType(ContentService.MimeType.TEXT);
}
