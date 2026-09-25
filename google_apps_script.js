function doPost(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  var data = JSON.parse(e.postData.contents);
  
  sheet.appendRow([
    new Date(),
    data.id,
    data.name,
    data.timestamp,
    data.status
  ]);
  
  return ContentService.createTextOutput("Success");
}