void ledblink(int i, int wacht) {
  for(int x=0; x<i; x++) {
    digitalWrite(led_onb, LED_ON);
    pixelsAan(100);
    delay(wacht);
    digitalWrite(led_onb, LED_OFF);
    pixelsAan(0);
    delay(wacht);
   }
}
void pixelsAan(int haha) {
    pixels->setPixelColor(2, pixels->Color(haha, haha, haha));
    pixels->show();
}

void consoleOut(String toLog) {
// decide to log to serial or the console 
  if(settings.diagNose) 
  {
    Serial.println(toLog);
  } 
}


String getChipId(bool sec) {
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  if(sec) return String(chipId); else return "ESP32-SWITCH-" + String(chipId);
}

bool loginBoth(String who) {
  const char* realm = "Login Required";
  bool authenticated = false;

  if (who == "admin") {
    realm = "Admin Login";
    authenticated = server.authenticate("admin", settings.passwd); 
  } 
  else if (who == "both") {
    realm = "User or Admin Login";
    // Check beide accounts
    authenticated = server.authenticate("admin", settings.passwd) || 
                    server.authenticate("user", settings.userPwd);
  }

  if (!authenticated) {
    // Toon de popup in de browser
    server.requestAuthentication(BASIC_AUTH, realm, "Login failed!");
    server.send(401, "text/html", "<html><body><h1>access denied</h1><p>Click <a href='/'>here</a> to go back.</p></body></html>");
    return false; // Toegang geweigerd
  }
  
  return true; // Toegang verleend
}

void calculateHueSat(uint8_t r, uint8_t g, uint8_t b) {
    float fr = r / 255.0f, fg = g / 255.0f, fb = b / 255.0f;
    float maxVal = max(fr, max(fg, fb)), minVal = min(fr, min(fg, fb));
    float delta = maxVal - minVal;

    // Bereken Hue (0-359)
    float h = 0;
    if (delta > 0) {
        if (maxVal == fr) h = 60 * fmod(((fg - fb) / delta), 6);
        else if (maxVal == fg) h = 60 * (((fb - fr) / delta) + 2);
        else if (maxVal == fb) h = 60 * (((fr - fg) / delta) + 4);
        if (h < 0) h += 360;
    }
    strip_hue = (int)h;

    // Bereken Saturation (0-100)
    strip_sat = (maxVal == 0) ? 0 : (delta / maxVal) * 100;
}