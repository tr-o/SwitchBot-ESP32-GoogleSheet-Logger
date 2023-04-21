function doPost(e) {
  const url = "https://docs.google.com/spreadsheets/d/****/edit#gid=0";
  const ss = SpreadsheetApp.openByUrl(url);
  const sheet = ss.getSheetByName("Data");
  const deviceCount = parseInt(e.parameter.DEVICE_COUNT);

  let rowData = [new Date()];

  for (let i = 0; i < deviceCount; i++) {
    rowData.push(
      e.parameter["Temp" + i],
      e.parameter["Hum" + i],
      e.parameter["Batt" + i]
    );
  }

  sheet.appendRow(rowData);
  return ContentService.createTextOutput('success');
}
