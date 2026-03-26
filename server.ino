
void startServer() 
{

  Serial.print("\nConnected to wifi, IP = ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", HTTP_GET, []() {
    if(!loginBoth("both")) return;
    String toSend = home_html;
    server.send(200, "text/html", toSend);
  });  
  
  server.on("/STYLES", HTTP_GET, []() {
  server.send_P(200, "text/css", STYLES);
  });
  
  server.on("/favicon.ico", []() {
  const char* favicon_base64 = "AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAu29YA0Q8YALTd3gBm4eMA0+blAIHh4wCa2tsAdd/gALUUnQC24+IAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAARERERERERERJkZmRGZkZlEkhIhIhIhKUSSFmFmFmEpRJIVUVUVUSlEkhV3EXcRGUSSZQAAB1YpRJJlczMHVilEiGWIOAdWKESIZYgIh1aIRIhliHiIiIhEiIiIVYhYiUSIZohmaCgpRIgiiCIoiClEiJmImZmJmUREREREREREQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
  server.send(200, "image/x-icon", favicon_base64); 
  // Let op: Sommige browsers accepteren Base64 direct als 'text/plain', 
  // maar voor een .ico is een echte binaire buffer beter.
});
  
  
  server.on("/MENU", HTTP_GET, []() {
  // we cannot open the menu from outside our own network.  
  if(checkRemote(server.client().remoteIP().toString()) ) 
  {
    // Send a redirect header to the browser
    server.sendHeader("Location", "/DENIED");
    server.send(302, "text/plain", ""); // 302 = Found / Temporary Redirect
    return; // Stop handling the rerequest
  }
  if(!loginBoth("admin")) return;
  
  String toSend = menu_html;
  server.send(200, "text/html", toSend);
  });  
  
  server.on("/SETTINGS", HTTP_GET, []() {
  if(!loginBoth("admin")) return;
  handleSettings();
  });

  server.on("/saveSettings", handleSaveSettings);
  
  server.on("/LOGPAGE", handleLogPage);

  server.on("/timer", handleTimer); 
  
  server.on("/TIMERCONFIG", HTTP_GET, []() {
  if(!loginBoth("admin")) return;
  zendpageTimers();
  });
    
  server.on("/submitTimers", handleTimerSave); 
 
  server.on("/REBOOT", HTTP_GET, []() {
     procesId = 4; // wait extra long
     confirm("/");
     //consoleOut("New dimmer_state: " + String(dimmer_state));
     delay(200);
     ESP.restart();
     }); 
   
   //Endpoint to request current status
   server.on("/get.Data", HTTP_GET, []() {
      // We build the JSON string
      String json = "{";
      json += "\"onoff\":" + String(strip_onoff ? "1" : "0");
      json += ",\"state\":" + String(strip_state);
      json += ",\"level\":" + String(strip_level);
      json += ",\"hue\":" + String(strip_hue);
      json += ",\"sat\":" + String(strip_sat);
      json += "}";
      server.send(200, "application/json", json); // Merk op: "application/json"
    });
   
   server.on("/buttons", []() {
     // which button is pressed
     int btn = server.arg("button").toInt();
     consoleOut("button pressed nr " + String(btn));
     if(btn != 0) strip_onoff = true; //als een knop (anders dan 0) is gedrukt dan altijd aan    
     switch(btn) {
       case 0:
          //strip_onoff = false; // clicking this button means always off 
          stripOff(1);
          UpdateLog(3, "switched off");
          break;
       case 1:
          setWhiteTone(1);
          //strip_state = 1;
          break;
       case 2:
          setWhiteTone(2);
          //strip_state = 2;
          break;
       case 3:
          setWhiteTone(3);
          //strip_state = 3;
          break;
       case 4:
          strip_state = 4;
          //take the saved values for sat and hue
          if(settings.phue != 0) strip_hue = settings.phue; 
          if(settings.psat != 0) strip_sat = settings.psat;
          setStripOn(); // on at once and inform rm and mqtt
          break;          
        case 5:
          strip_state = 5;
          consoleOut("prog_1");
          setStripOn(); // on at once and inform rm and mqtt
          break;
      }
      if(strip_onoff) UpdateLog(5, "switched on"); else UpdateLog(5, "switched off");
      //update_strip(true, true);
      server.send(200, "text/plain", "buttons OK");
 });

   server.on("/sliders", []() {
     // which slider is moved
     int sld = server.arg("slider").toInt();
     int val = server.arg("val").toInt();
     consoleOut("slider shifted to " + String(val));
     switch(sld) {
       case 1:
          strip_hue = val;
          strip_state = 4;
          setHue(1);
          raiseFlag(FLAG_RM_HUE);   // Zet Rainmaker Hue vlag aan
          break;
       case 2:
          strip_sat = val;
          strip_state = 4;
          setSat(1);
          raiseFlag( FLAG_RM_SAT );   // Zet Rainmaker Hue vlag aan
          break;
       case 3:
          strip_level = val;
          if(strip_level == 0) stripOff(1); else setDim(1);
     }

     // when a slider is handled, the light goes on
     //update_strip(true, true);
     server.send(200, "text/plain", "sliders OK");
 });


   server.on("/DENIED", HTTP_GET, []() {
   server.send_P(200, "text/html", "<center><h2>FORBIDDEN");
   });
   server.on("/ABOUT", handleAbout);
   server.onNotFound(handleNotFound);
   Serial.println("HTTP server started");
   server.begin();
}


void confirm(String dest) {
//if(device) snprintf(requestUrl, sizeof(requestUrl), "/DEV?welke=%d", devChoice);
String cont  = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta charset='utf-8'><script>";
cont += "let waitTime=" + String(3000*procesId) + ";";
cont += "function redirect(){";
cont += "let counter=document.getElementById('counter');";
cont += "let secs=waitTime/1000;";
cont += "counter.textContent=secs;";
cont += "let timer=setInterval(function(){";
cont += "secs--; counter.textContent=secs;";
cont += "if(secs<=0){ clearInterval(timer); window.parent.location.href='" + dest + "'; }";
cont += "},1000);";
cont += "}";
cont += "</script></head>";
cont += "<body onload='redirect()' style='background-color: #edd8f0;'>";
cont += "<br><br><center><h1>processing<br>your request,<br>please wait</h1><br><br>";
cont += "<h3>Redirecting in <span id='counter'></span> seconds...</h3></center>";
cont += "</body></html>";
server.send(200, "text/html", cont);
}