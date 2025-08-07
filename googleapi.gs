function doGet(e) {
  const spreadsheetId = '15ODUqaZt0XR8L83GgnU2j2FXJ6Nciul-Q-PU3takrYw';
  const met = e.parameter.met;
  if(met == 0){ // GET Card REQUIRES Password
    try {
      const values = Sheets.Spreadsheets.Values.get(spreadsheetId, 'StudentID!A2:B1000').values;
      const password = Sheets.Spreadsheets.Values.get(spreadsheetId, "Password!A1").values;
      if (!values || !password){
        return ContentService.createTextOutput("ERROR");
      }
      if(e.parameter.pass != password[0][0]) {
        return ContentService.createTextOutput("VERIFY");
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
  }else if(met == 1){ // POST Log REQUIRES Verification code
    try {
      const values = Sheets.Spreadsheets.Values.get(spreadsheetId, "Log!F1").values;
      const emails = Sheets.Spreadsheets.Values.get(spreadsheetId, "Email!A1").values;
      if (!values) {
        return ContentService.createTextOutput("ERROR");
      }
      if (!emails) {
        return ContentService.createTextOutput("ERROR");
      }
      if(e.parameter.verify != generateCode()) {
        return ContentService.createTextOutput("VERIFY");
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
        var sortingRange = sheet.getRange(2, 1, Number(values[0][0]), 5);
        sortingRange.sort([4, 3, 2]);
        SpreadsheetApp.flush();
        MailApp.sendEmail(
          emails[0][0],
          'New Sign '+inout+': '+name+', '+reason,
          'Sign: '+inout+'\nName: '+name+'\nReason: '+reason,
        );
        return ContentService.createTextOutput("Success");
      } catch (err) {
        console.log('Failed with error %s', err.message);
      }
    } catch (err) {
      console.log(err.message);
    }
  }else if(met == 2){ // POST Card REQUIRES Password
    try {
      const values = Sheets.Spreadsheets.Values.get(spreadsheetId, "StudentID!C1").values;
      const password = Sheets.Spreadsheets.Values.get(spreadsheetId, "Password!A1").values;
      if (!values || !password) {
        return ContentService.createTextOutput("ERROR");
      }
      if(e.parameter.pass != password[0][0]) {
        return ContentService.createTextOutput("VERIFY");
      }
      try {
        var ss = SpreadsheetApp.openById(spreadsheetId);
        var sheet = ss.getSheetByName('StudentID');
        const range = sheet.getRange(Number(values[0][0])+1, 1, 1, 2);
        range.setValues([[e.parameter.name, e.parameter.data]]);
        SpreadsheetApp.flush();
        return ContentService.createTextOutput("Success");
      } catch (err) {
        console.log('Failed with error %s', err.message);
      }
    } catch (err) {
      console.log(err.message);
    }
  }else if(met == 3){ // GET Password & Verify code REQUIRES Password
    try {
      const password = Sheets.Spreadsheets.Values.get(spreadsheetId, "Password!A1").values;
      if (!password) {
        return ContentService.createTextOutput("ERROR");
      }
      try {
        if(e.parameter.pass == password[0][0]){
          return ContentService.createTextOutput("True"+generateCode());
        }else{
          return ContentService.createTextOutput("False");
        }
      } catch (err) {
        console.log('Failed with error %s', err.message);
      }
    } catch (err) {
      console.log(err.message);
    }
  }
  return ContentService.createTextOutput("ERROR");
}

//PRNG code
function cyrb128(str) {
  let h1 = 1779033703, h2 = 3144134277,
    h3 = 1013904242, h4 = 2773480762;
  for (let i = 0, k; i < str.length; i++) {
    k = str.charCodeAt(i);
    h1 = h2 ^ Math.imul(h1 ^ k, 597399067);
    h2 = h3 ^ Math.imul(h2 ^ k, 2869860233);
    h3 = h4 ^ Math.imul(h3 ^ k, 951274213);
    h4 = h1 ^ Math.imul(h4 ^ k, 2716044179);
  }
  h1 = Math.imul(h3 ^ (h1 >>> 18), 597399067);
  h2 = Math.imul(h4 ^ (h2 >>> 22), 2869860233);
  h3 = Math.imul(h1 ^ (h3 >>> 17), 951274213);
  h4 = Math.imul(h2 ^ (h4 >>> 19), 2716044179);
  h1 ^= (h2 ^ h3 ^ h4), h2 ^= h1, h3 ^= h1, h4 ^= h1;
  return [h1>>>0, h2>>>0, h3>>>0, h4>>>0];
}
function splitmix32(a) {
  return function() {
    a |= 0;
    a = a + 0x9e3779b9 | 0;
    let t = a ^ a >>> 16;
    t = Math.imul(t, 0x21f0aaad);
    t = t ^ t >>> 15;
    t = Math.imul(t, 0x735a2d97);
    return ((t = t ^ t >>> 15) >>> 0) / 4294967296;
  }
}
function convertBase(value, from_base, to_base) {
  var range = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'.split('');
  var from_range = range.slice(0, from_base);
  var to_range = range.slice(0, to_base);
  var dec_value = value.split('').reverse().reduce(function (carry, digit, index) {
    if (from_range.indexOf(digit) === -1) throw new Error('Invalid digit `'+digit+'` for base '+from_base+'.');
    return carry += from_range.indexOf(digit) * (Math.pow(from_base, index));
  }, 0);
  var new_value = '';
  while (dec_value > 0) {
    new_value = to_range[dec_value % to_base] + new_value;
    dec_value = (dec_value - (dec_value % to_base)) / to_base;
  }
  return new_value || '0';
}
function generateCode(){
  var date = new Date(); 
  var seed = cyrb128(Utilities.formatDate(date, "Pacific/Auckland", "d/M/y"));
  var rand = splitmix32(seed[0]);
  for(let i=0; i<10; i++) rand();
  return convertBase(String((Math.floor(rand()*Number.MAX_SAFE_INTEGER/10))), 10, 36).slice(-4);
}

//Clear function
function clear(){
  const spreadsheetId = '15ODUqaZt0XR8L83GgnU2j2FXJ6Nciul-Q-PU3takrYw';
  try {
    var ss = SpreadsheetApp.openById(spreadsheetId);
    var sheet = ss.getSheetByName('Log');
    const range = sheet.getRange('A2:E10000');
    range.clear();
    SpreadsheetApp.flush();
  } catch (err) {
    console.log('Failed with error %s', err.message);
  }
}
