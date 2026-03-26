bool mqttConnect() {   // 
/* this function checks if we are connected to the broker, if not connect anyway */  
   
    if( MQTT_Client.connected() ) {
    consoleOut("mqtt was connected");
    return true;
    }
     consoleOut(F("\nconnect mqtt"));
    // we are here because w'r not connected. Signal with the LED
    //ledblink(2,70);

    //if (Mqtt_Port == 0 ) { Mqtt_Port = 1883;}   // just in case ....
    uint8_t retry = 3;
    
    //char Mqtt_inTopic[11]={"ESP-ECU/in"};

    while (!MQTT_Client.connected()) {

      if ( MQTT_Client.connect( settings.Mqtt_Clientid, settings.Mqtt_Username, settings.Mqtt_Password) )
      {
         //connected, so subscribe to inTopic (not for thingspeak)
        if( settings.Mqtt_Format != 0 ) {
           //String sub = "ESP32-" + getChipId(true) + "/in"; // to get a intopic ESP32-234567/in
           if( MQTT_Client.subscribe ( settings.Mqtt_inTopic ) ) {
               consoleOut("subscribed to " + String(settings.Mqtt_inTopic));
           }
        }
         consoleOut(F("mqtt connected"));
         UpdateLog(3, "connected");
      
       return true;

    } else {
        //String term = "connection failed state: " + String(MQTT_Client.state());
        UpdateLog(3, "failed");
        if (!--retry) break; // stop when tried 3 times
        delay(500);
    }
  }
  // if we are here , no connection was made.

  consoleOut(F("mqtt connection failed"));
  return false;
}

// *************************************************************************
//                   process received mqtt
// *************************************************************************

void MQTT_Receive_Callback(char *topic, byte *payload, unsigned int length)
{
    // domoticz messages can be long so the buffer should be large enough (setup)
    int idx = 0;
    uint8_t nLevel = 0;
    consoleOut(F("received mqtt"));
    JsonDocument doc;       // We use json library to parse the payload 
    String jsonString = "";                        
   //  The function deserializeJson() parses a JSON input and puts the result in a JsonDocument.
   // DeserializationError error = deserializeJson(doc, Payload); // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, payload); // Deserialize the JSON document
    if (error)            // Test if parsing succeeds.
    {
       consoleOut("mqtt no valid json ");
        return;
    } 
    
    /* {
        "Color" :
        {
                "b" : 62,
                "cw" : 0,
                "g" : 255,
                "m" : 3,
                "r" : 227,
                "t" : 0,
                "ww" : 0
        },
        "Level" : 50,
        "idx" : 986,
       }  */
    
    // the json contains a lot, a.o. idx and "svalue1" : "72" and "level":72,
    // we need Level, idx,  
    // We check the kind of command format received with MQTT
    //now we have a payload like {"switch",on}    
    if(!doc["idx"].isNull())
    {
      idx = doc["idx"].as<int>();
      //now lookup the device
      
      if(idx == settings.Mqtt_switchIDX) {
        consoleOut("recognized idx " + String(idx)) ;
      } else {
        consoleOut("wrong idx, abort");
        return;
      }
    }  //consoleOut("devNr " + String(dev)) ;
    
    
    if (!doc["Level"].isNull()) 
      { 
         strip_level = doc["Level"].as<int>(); 
         
        if ( strip_level < 2 ) // level won't get under 1
        {
            stripOff(3);
        } else {
           raiseFlag( FLAG_RM_DIM);
        }
      }
    
    // check got nvalue (on.off) 
    if (doc["nvalue"].is<int>()) 
    { 
       strip_onoff = (doc["nvalue"].as<int>() > 0); 
       if(strip_onoff == false) 
       {
           stripOff(3);
           return;
       }
    }
    
    if(settings.scene != 0)
    {
    //we need to set the scene and skip the color settings below  
       consoleOut("mqtt setting predefined scene");
       setScene();
       raiseFlag( FLAG_RM_HUE | FLAG_RM_SAT | FLAG_RM_DIM | FLAG_RM_POW);
       return;
    }
    consoleOut("mqtt set strip on");
    
    if (!doc["Color"].isNull())
    {
       uint8_t r = doc["Color"]["r"];  // 255
       uint8_t g = doc["Color"]["g"];  // 7
       uint8_t b = doc["Color"]["b"];  // 59

      // calculate RGB back to the slider values
      calculateHueSat(r, g, b);
      // if the strip is on and hueflow then only change level
      if(strip_state != 5 ) strip_state = 4;
      setStripColor(); // on at once
      raiseFlag( FLAG_RM_HUE | FLAG_RM_SAT | FLAG_RM_POW);
    }
    //update_strip(false, true);
}

bool mqttGeldig() {
    if (settings.Mqtt_Broker[0] == '\0' || settings.Mqtt_Format == 0) {
    return false;
    }
    if (settings.Mqtt_switchIDX == 0) { // geen geldige idx
    return false;   
    } else {
    return true; 
    }
}

void send_mqttSwitch() 
{
  if ( !mqttGeldig() ){ 
    consoleOut("no valid mqtt address or not configured, skipping..");
    return;
  }    

// update switch state
   char out[124];
  JsonDocument doc;
  doc["command"] = "switchlight";
  doc["idx"] = settings.Mqtt_switchIDX;
  if(settings.Mqtt_Format == 1) {
  doc["switchcmd"] = (strip_onoff == 0) ? "Off" : "On";
  } else {
  (strip_onoff == 0) ? doc["state"] = "off" : doc["state"] = "on";  
  }
  int b =serializeJson(doc, out);
  MQTT_Client.publish (settings.Mqtt_outTopic, out );

}

  void send_mqttColor() 
  {
    if ( !mqttGeldig() ){ 
      consoleOut("no valid mqtt address or not configured, skipping..");
      return;
    }    

    if(settings.Mqtt_Format == 1) {
    // 1. Bereken de kleur (we gebruiken 255 voor brightness, 
    // want Domoticz krijgt brightness in een apart veld)
    uint32_t hue_32 = map(strip_hue, 0, 360, 0, 65535);
    uint8_t sat_8 = map(strip_sat, 0, 100, 0, 255);
    
    // Converteer naar een 32-bit RGB waarde (0x00RRGGBB)
    uint32_t temp_rgb = pixels->ColorHSV(hue_32, sat_8, 255);

    // 2. Trek de RGB waarden uit de 32-bit integer
    uint8_t r = (uint8_t)(temp_rgb >> 16);
    uint8_t g = (uint8_t)(temp_rgb >> 8);
    uint8_t b = (uint8_t)temp_rgb;
    
    
    // update color state
    char out[124];
    JsonDocument doc;
    doc["command"] = "setcolbrightnessvalue";
    doc["idx"] = settings.Mqtt_switchIDX;
    doc["brightness"] = strip_level; // Waarde 0-100
    // Maak het kleur-object aan
    JsonObject color = doc["color"].to<JsonObject>();
    color["m"] = 3; // Modus 3 = RGB
    color["t"] = 0; // Temperatuur (niet relevant voor RGB, dus 0)
    color["r"] = r; // Je berekende R (0-255)
    color["g"] = g; // Je berekende G (0-255)
    color["b"] = b; // Je berekende B (0-255)
   
    int bah =serializeJson(doc, out);
    MQTT_Client.publish (settings.Mqtt_outTopic, out );

    }
  }