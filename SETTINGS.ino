
void handleSettings() {
  // Voor eenvoud in de standaard WebServer library:
  String toSend = settings_html; // Kopieer uit PROGMEM naar RAM
  // Vervang handmatig de tags (of gebruik de AsyncWebServer library voor een snellere 'processor')
// --- Strings (Char Arrays) ---
  toSend.replace("%DVNAME%",   String(settings.dvName));
  toSend.replace("%PASSWD%",   String(settings.passwd));
  toSend.replace("%USERPWD%",  String(settings.userPwd));
  toSend.replace("%GMT%",      String(settings.gmtOffset));
  toSend.replace("%MQBRK%",    String(settings.Mqtt_Broker));
  toSend.replace("%MQUSR%",    String(settings.Mqtt_Username));
  toSend.replace("%MQPWD%",    String(settings.Mqtt_Password));
  toSend.replace("%MQCID%",    String(settings.Mqtt_Clientid));
  toSend.replace("%MQIN%",     String(settings.Mqtt_inTopic));
  toSend.replace("%MQOUT%",    String(settings.Mqtt_outTopic));

  // --- Getallen (Int / Float / Uint8) ---
  toSend.replace("%LONGI%",    String(settings.longi, 3)); // 3 decimalen
  toSend.replace("%LATI%",     String(settings.lati, 3));
  toSend.replace("%SECLVL%",   String(settings.securityLevel));
  toSend.replace("%DUTY%",     String(settings.default_duty));
  toSend.replace("%MQPRT%",    String(settings.Mqtt_Port));
  toSend.replace("%MQFMT%",    String(settings.Mqtt_Format));
  toSend.replace("%MQIDX%",    String(settings.Mqtt_switchIDX));
  toSend.replace("%GMT%",      String(settings.gmtOffset));
  // --- Booleans (Checkboxes) ---
  toSend.replace("%DTS_CHK%",  settings.DTS ? "checked" : "");
  toSend.replace("%DIAG_CHK%", settings.diagNose ? "checked" : "");

  // ... herhaal voor de rest ...
  server.send(200, "text/html", toSend);
}

void handleSaveSettings()
{
if (server.method() != HTTP_POST) { server.send(405, "text/plain", "Method Not Allowed"); return; }

  // Strings (Char arrays)
  strncpy(settings.dvName, server.arg("dvName").c_str(), 20);
  strncpy(settings.passwd, server.arg("passwd").c_str(), 10);
  strncpy(settings.userPwd, server.arg("userPwd").c_str(), 10);
  strncpy(settings.gmtOffset, server.arg("gmt").c_str(), 4);
  strncpy(settings.Mqtt_Broker, server.arg("mqBrk").c_str(), 29);
  strncpy(settings.Mqtt_Username, server.arg("mqUsr").c_str(), 25);
  strncpy(settings.Mqtt_Password, server.arg("mqPwd").c_str(), 25);
  strncpy(settings.Mqtt_Clientid, server.arg("mqCID").c_str(), 25);
  strncpy(settings.Mqtt_inTopic, server.arg("mqIn").c_str(), 25);
  strncpy(settings.Mqtt_outTopic, server.arg("mqOut").c_str(), 25);

  // Getallen (Int / Float / Uint8)
  settings.longi = server.arg("longi").toFloat();
  settings.lati = server.arg("lati").toFloat();
  settings.securityLevel = (uint8_t)server.arg("secLvl").toInt();
  settings.default_duty = server.arg("duty").toInt();
  settings.Mqtt_Port = server.arg("mqPrt").toInt();
  settings.Mqtt_Format = server.arg("mqFmt").toInt();
  settings.Mqtt_switchIDX = server.arg("mqIdx").toInt();
  // Booleans (Checkboxes)
  settings.DTS = server.hasArg("DTS");
  settings.diagNose = server.hasArg("diag");
  saveSettings(); // save with preferences
  confirm("/SETTINGS");
  //server.send(200, "text/html", toSend);

  // Optioneel: Als je de ESP32 wilt herstarten na opslaan, doe dat dan na een delay
  // delay(1000); 
  // ESP.restart(); 

}
