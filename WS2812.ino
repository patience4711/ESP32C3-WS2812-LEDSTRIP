void stripStart() {
    Serial.println("\nledstrip starting");
    pixels = new Adafruit_NeoPixel(settings.numpix, gpio_strip, NEO_GRB + NEO_KHZ800);
    pixels->begin();
    
    // 4. Test of het werkt
    pixels->clear();
    pixels->show();
}

void setWhiteTone(uint8_t tone) {
  uint32_t color;
  
  if (tone == 1) {
    color = pixels->Color((255*strip_level)/100, (197*strip_level)/100, (143*strip_level)/100);
  } else if (tone == 2) {
    color = pixels->Color((255*strip_level)/100, (255*strip_level)/100, (214*strip_level)/100);
  } else { // cold
    color = pixels->Color((201*strip_level)/100, (226*strip_level)/100, (255*strip_level)/100);
  }
  pixels->fill(color); // Zet alle pixels op deze kleur
  pixels->show();
}

void update_strip() 
{
  if (!strip_onoff) {
    consoleOut("update_strip, must switch off");
    pixels->clear(); // Maakt intern alle pixels zwart
    checkTimers(); // disarm an eventual switched timer
    my_device->updateAndReportParam("Power", strip_onoff);
    // Hier hoeft verder niets, de pixels.show() onderaan doet het werk
  } 
  else {
    // Rapporteer AAN en Brightness naar RainMaker
    my_device->updateAndReportParam("Power", strip_onoff);
    my_device->updateAndReportParam("Brightness", (int)strip_level);
    
    if (strip_state >= 1 && strip_state <= 3) {   // 1 2 3
      setWhiteTone(strip_state); 
      // Rapporteer Hue en Saturation naar RainMaker
      my_device->updateAndReportParam("Hue", (int)strip_hue);
      my_device->updateAndReportParam("Saturation", (int)strip_sat);
      return;    
    } 
    else if (strip_state == 4) { // GEBRUIK DUBBELE ==
      consoleOut("input values for update_strip");
      consoleOut("hue " + String(strip_hue));
      consoleOut("sat " + String(strip_sat));
      consoleOut("level " + String(strip_level));
      
      // Rapporteer Hue en Saturation naar RainMaker
      my_device->updateAndReportParam("Hue", (int)strip_hue);
      my_device->updateAndReportParam("Saturation", (int)strip_sat);
      uint32_t scaled_hue = map(strip_hue, 0, 360, 0, 65535);
      uint8_t scaled_sat = map(strip_sat, 0, 100, 0, 255);
      uint8_t scaled_bri = map(strip_level, 0, 100, 0, 255);

      uint32_t rgbcolor = pixels->ColorHSV(scaled_hue, scaled_sat, scaled_bri);
      pixels->fill(rgbcolor);
    } 
    else {
      consoleOut("prog_1");
      return;
    }
  }
  pixels->show(); // Deze stuurt de 'clear' of de 'color' pas echt naar de strip
  send_mqttSwitch();
  send_mqttColor();
}

void prog_1()
{
      currentHue += 25;
      //if(currentHue > 359) currentHue = 0;
      uint32_t  rgbcolor = pixels->ColorHSV(currentHue, 255, strip_level);
      pixels->fill(rgbcolor);
      pixels->show();
      
}