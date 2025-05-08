function doGet(e) {
  const spreadsheetId = '15ODUqaZt0XR8L83GgnU2j2FXJ6Nciul-Q-PU3takrYw';
  // e.parameter.email
  try {
    const rangeName2 = 'Log!A1:D20';
    const values = Sheets.Spreadsheets.Values.get(spreadsheetId, rangeName2).values;
    if (!values) {
      console.log('No data found.');
      return;
    }
    console.log('Data:');
    for (const row in values) {
      console.log('%s', values[row][0]);
      if(values[row][0] == email){
        rowtochange = row;
      }
    }
    if(rowtochange != -1){
      return ContentService.createTextOutput(values[rowtochange][1]).setMimeType(ContentService.MimeType.TEXT);
    }else{
      return ContentService.createTextOutput("ERROR").setMimeType(ContentService.MimeType.TEXT);
    }
  } catch (err) {
    console.log(err.message);
  }
}
