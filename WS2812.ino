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
  strip_state = tone;
  if (tone == 1) {
    color = pixels->Color((255*strip_level)/100, (197*strip_level)/100, (143*strip_level)/100);
    strip_hue = 32;      // Warm oranje/geel
    strip_sat = 50;      // Lage verzadiging voor de 'witte' look
  } else if (tone == 2) {
    strip_hue = 45;      // Warm oranje/geel
    strip_sat = 10;      // Lage verzadiging voor de 'witte' look
    //color = pixels->Color((255*strip_level)/100, (255*strip_level)/100, (214*strip_level)/100);
  } else { // cold
    strip_hue = 195;      // Warm oranje/geel
    strip_sat = 20;      // Lage verzadiging voor de 'witte' look
    //color = pixels->Color((201*strip_level)/100, (226*strip_level)/100, (255*strip_level)/100);
  }
  //strip_level = 80;   // Volledige helderheid
  strip_onoff = true;
  setStripOn();
  //pixels->fill(color); // Zet alle pixels op deze kleur
  //pixels->show();
  // these scenes are aonly set by the webui so we always update rm and mqtt
  raiseFlag(FLAG_RM_HUE | FLAG_RM_SAT | FLAG_MQTT_COL | FLAG_MQTT_SW);
}


 
 void setStripOn() //only used by webui and e
  {
      setStripColor();
   // we inform rm and mqtt 
        raiseFlag(FLAG_RM_HUE | FLAG_RM_SAT | FLAG_RM_DIM | FLAG_RM_POW | FLAG_MQTT_COL | FLAG_MQTT_SW);
  }


void stripOff(uint8_t who)
{
    //in any case the strip goes out
    strip_onoff = false;
    consoleOut("strip switch off");
    pixels->clear(); // Maakt intern alle pixels zwart
    checkTimers(); // disarm an eventual switched timer
    pixels->show();
    // determine which updates should be sent
    switch(who)
    {
      case 1:
        // the webui
        // we should send updates to rm and mqtt
        raiseFlag(FLAG_RM_POW | FLAG_MQTT_COL | FLAG_MQTT_SW);
        //UpdateLog(3, "switched off");
        break;
    case 2:
        // rainmaker
        // we should send updates to mqtt
        raiseFlag(FLAG_MQTT_COL | FLAG_MQTT_SW);
        UpdateLog(4, "switched off");
        break;
    case 3:
        // mqtt
        // we should send updates to rm
        UpdateLog(3, "switched off");
        raiseFlag(FLAG_RM_POW); // Zet MQTT Kleur vlag aan
        break;
    }
}


void setHue(uint8_t who)
{
 // in any case we set the color, that means it goes on.
 setStripColor(); 
 strip_onoff = true;
 switch(who)
 {
    case 1:
        // the slider in the webui
        // we should send updates to rm and mqtt
        raiseFlag(FLAG_RM_HUE | FLAG_MQTT_COL | FLAG_MQTT_SW);
        break;
    case 2:
        // rainmaker
        // we should send updates to mqtt
        raiseFlag(FLAG_MQTT_COL | FLAG_MQTT_SW);
        break;
    case 3:
        // mqtt
        // we should send updates to rm
        raiseFlag(FLAG_RM_HUE);
        break;
 } 

}

void setSat(uint8_t who)
{
 // in any case we set the color, that means it goes on
 setStripColor(); 
 strip_onoff = true;
 switch(who)
 {
    case 1:
        // the slider in the webui
        // we should send updates to rm and mqtt
        raiseFlag(FLAG_RM_SAT | FLAG_MQTT_COL | FLAG_MQTT_SW);
        break;
    case 2:
        // rainmaker
        // we should send updates to mqtt
        raiseFlag(FLAG_MQTT_COL | FLAG_MQTT_SW);
        break;
    case 3:
        // mqtt
        // we should send updates to rm
        raiseFlag(FLAG_RM_SAT);
        break;
 } 

}

void setDim(uint8_t who)
{
 // in any case we set the brightness
 setStripColor(); 
 switch(who)
 {
    case 1:
        // the slider in the webui
        // we should send updates to rm and mqtt
        raiseFlag(FLAG_RM_DIM | FLAG_MQTT_COL | FLAG_MQTT_SW);
        break;
    case 2:
        // rainmaker
        // we should send updates to mqtt
        raiseFlag(FLAG_MQTT_COL | FLAG_MQTT_SW);
        break;
    case 3:
        // mqtt
        // we should send updates to rm
        raiseFlag( FLAG_RM_DIM) ;   // Zet Rainmaker Sat vlag aan
        break;
 } 

}
void setStripColor()
{
      uint32_t scaled_hue = map(strip_hue, 0, 360, 0, 65535);
      uint8_t scaled_sat = map(strip_sat, 0, 100, 0, 255);
      uint8_t scaled_bri = map(strip_level, 0, 100, 0, 255);

      uint32_t rgbcolor = pixels->ColorHSV(scaled_hue, scaled_sat, scaled_bri);
      pixels->fill(rgbcolor);
      pixels->show();
}

void rmUpdateHue() { my_device->updateAndReportParam("Hue", (int)strip_hue); }
void rmUpdateSat() { my_device->updateAndReportParam("Saturation", (int)strip_sat); }
void rmUpdateDim() { my_device->updateAndReportParam("Brightness", (int)strip_level); }
void rmUpdatePow() { my_device->updateAndReportParam("Power", strip_onoff); }

void prog_1()
{
      currentHue += 25;
      //if(currentHue > 359) currentHue = 0;
      uint32_t  rgbcolor = pixels->ColorHSV(currentHue, 255, strip_level);
      pixels->fill(rgbcolor);
      pixels->show();
}

void raiseFlag(uint16_t flag) {
    updateRequests |= flag;   // Zet de specifieke bit(s) aan
    lastChangeTime = millis(); // Reset de timer
}

void setScene()  // 
{
   strip_onoff = true; //als een knop (anders dan 0) is gedrukt dan altijd aan    
   strip_state = settings.scene;
     switch(strip_state) {
       case 0:
          return;
       case 1:
          setWhiteTone(1);
          break;
       case 2:
          setWhiteTone(2);
          break;
       case 3:
          setWhiteTone(3);
          break;
       case 4:
          //if the value is not 0 we take the saved values for sat and hue
          if( settings.phue != 0 ) strip_hue = settings.phue; 
          if( settings.psat != 0 ) strip_sat = settings.psat;
          setStripColor(); // on at once and inform rm and mqtt
          break;          
        case 5:
          consoleOut("prog_1");
          setStripOn(); // on at once and inform rm and mqtt
          break;
      }
    
     //f(strip_onoff) UpdateLog(who, "switched on"); else UpdateLog(who, "switched off");
      //update_strip(true, true);

 }