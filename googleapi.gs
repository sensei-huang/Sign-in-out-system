function doGet(e) {
  const spreadsheetId = '15ODUqaZt0XR8L83GgnU2j2FXJ6Nciul-Q-PU3takrYw';
  // e.parameter.email
  try {
    const rangeName = 'Log!A1:D20';
    const values = Sheets.Spreadsheets.Values.get(spreadsheetId, rangeName).values;
    if (!values) {
      console.log('No data found.');
      return;
    }
    console.log(values[0][1]);
    console.log('Data:');
    values.forEach(function(entry) {
      entry.forEach(function(entry2) {
        console.log(entry2);
      });
    });
    return ContentService.createTextOutput(values[0][0]);
    // return ContentService.createTextOutput("ERROR").setMimeType(ContentService.MimeType.TEXT);
  } catch (err) {
    console.log(err.message);
  }
}
