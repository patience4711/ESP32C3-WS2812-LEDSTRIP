//This example demonstrates the ESP RainMaker with a custom device
#include "AAA_INCLUDES.h"

Device *my_device_ptr;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// *                             s e t u p                                           *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void setup() {
  Serial.begin(115200);
  pinMode(onboard_button, INPUT_PULLUP);
  pinMode(gpio_strip, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(led_onb, OUTPUT);
  //digitalWrite(gpio_strip, DEFAULT_POWER_MODE);
  digitalWrite(led_onb, LED_ON);
  delay(500);
  
  loadSettings(); // from prefences
  
  
  Node my_node;
  my_node = RMaker.initNode("ESP RainMaker Node");
  // register for an event
  esp_event_handler_register(RMAKER_COMMON_EVENT, ESP_EVENT_ANY_ID, &mqtt_event_handler, NULL);
  
  my_device = new Device("ledstrip", ESP_RMAKER_DEVICE_LIGHTBULB, &gpio_strip);
  if (!my_device) {
    return;
  }
  
  my_device->addNameParam("Ledstrip");

  my_device->addPowerParam(DEFAULT_POWER_MODE);
  my_device->assignPrimaryParam(my_device->getParamByName(ESP_RMAKER_DEF_POWER_NAME));
  //Create and add a custom Brightness parameter
  Param brightness("Brightness",
                 ESP_RMAKER_PARAM_BRIGHTNESS,
                 value(DEFAULT_LEVEL),
                 PROP_FLAG_READ | PROP_FLAG_WRITE);
  // add properties to the brightness parameter
  brightness.addBounds(value(0), value(100), value(1));
  brightness.addUIType(ESP_RMAKER_UI_SLIDER);
  my_device->addParam(brightness);

  // //Create and add a custom level parameter
  // Param level_param("Level", "custom.param.level", value(DEFAULT_DIMMER_LEVEL), PROP_FLAG_READ | PROP_FLAG_WRITE);
  // level_param.addBounds(value(0), value(100), value(1));
  // level_param.addUIType(ESP_RMAKER_UI_SLIDER);
  // my_device->addParam(level_param);
  // 4. Hue (Kleurtoon: 0-360)
  my_device->addHueParam(DEFAULT_HUE);

  // 5. Saturation (Verzadiging: 0-100)
  my_device->addSaturationParam(DEFAULT_SATURATION);
  
  
  
  my_device->addCb(write_callback);

  //Add custom dimmer device to the node
  my_node.addDevice(*my_device);

  RMaker.enableTZService();

  RMaker.enableSchedule();

  RMaker.enableScenes();

  //RMaker.start();
  
  WiFi.onEvent(sysProvEvent);  // Will call sysProvEvent() from another thread.

  WiFiProv.beginProvision(NETWORK_PROV_SCHEME_BLE, NETWORK_PROV_SCHEME_HANDLER_FREE_BTDM, NETWORK_PROV_SECURITY_1, pop, service_name);
  delay(500);
  
   // this provides some time for the server to get in the air
    // we should be connected, else wait for connection
    //int counTer = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    delay(500);
    RMaker.start();

    //unsigned long timeout = 60000; // 60 seconden
    unsigned long start = millis();
    Serial.println("\nwaiting for association");
    while (millis() - start < 60000) {
    if(rm_associated) 
       {
          Serial.println("\nrainmaker connect success\n");
          break;
    }
      delay(1000);
      Serial.print("0");
    }

    loadTimers(); // from preferences
 
    delay(500);
    
    getTijd(); // retrieve time from the timeserver
     // we calculate the timers in the loop   
    
    stripStart();

    ledblink(3, 300);
    UpdateLog(1, "boot up");
    startServer();
    // make sure we have light when on boot 'on' is called
    strip_level = 50; // fall back value
    //if(settings.default_duty > 0 ) last_duty = settings.default_duty; 
  
    // ****************** mqtt init *********************
     MQTT_Client.setKeepAlive(150);
     MQTT_Client.setServer(settings.Mqtt_Broker, settings.Mqtt_Port);
     MQTT_Client.setBufferSize(1024);
     MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;

    if ( settings.Mqtt_Format != 0 ) 
    {
        mqttConnect(); // mqtt connect
    } 
  
  }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// *                             l o o p                                             *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void loop() {

    
      
      // --- Health check for time every 10 minutes---
    if (millis() - laatsteMeting > HEALTH_INTERVAL) {
        laatsteMeting = millis();
        consoleOut("running healthcheck");
        if (!timeRetrieved && WiFi.status() == WL_CONNECTED) {
            getTijd();  // will set timeRetrieved if successful
        }
    }


  // timers calculation
  if (day() != datum && timeRetrieved)
  {
    // this means we have a valid systemtime
    // we can calculate sunset and sunrise
    // datum set to day() to make this one shot
    {   
       sun_setrise();
       //arm the timers for recalculation
       mustCalc[0] = mustCalc[1] = mustCalc[2] = mustCalc[3] = true; 
       datum = day();
       resyncFlag = true;
    } 
  }

   // if retrieve fails, day will not be datum, so we keep trying by healthcheck
  if (resyncFlag && hour() > 4) // if date overflew and later then 2
  { 
    getTijd(); // retrieve time results in timeRetieved
    resyncFlag = false; 
  }
  
  /* timer calculation
   we calculate the timer only when there is a 
   - valid time, (time is retrieved)
   - it is off (hasSwitched = false)
   - it has not been done already (mustCalc = true)
   tinmerCalc always return with mustCalc = false because we only calculate
   when these conditions are met.
  */
  for(int r=0; r<TIMERCOUNT; r++)
  {   
    if(timeRetrieved && !hasSwitched[r] && mustCalc[r])  timerCalc(r);
  }
// ****************************************************************************
//                switch by the timers
// ****************************************************************************  
   schakelen();

  if(digitalRead(onboard_button) == LOW) read_onboard_button();

  if(digitalRead(button1) == LOW) check_button1();

  // ***************************************************************************
  //                       m o s q u i t t o
  // ***************************************************************************
  // before each transmission the connection is tested
  // so we don't do this in the loop
  if(settings.Mqtt_Format != 0 ) MQTT_Client.loop(); //looks for incoming messages
  
  //  SERIAL: *************** check if there is data on serial **********************
  if(Serial.available()) {
       handle_Serial();
   }
  
    // Alleen uitvoeren als de modus actief is EN het tijd is voor de volgende stap
  if (strip_state == 5 && strip_onoff) {
    if (millis() - lastUpdate > (11-settings.speed)*10) {
      prog_1();
      lastUpdate = millis();
    }
  }
   
    if (updateRequests > 0 && (millis() - lastChangeTime > debounceTime)) {
    consoleOut("handle flags = 0b" + String(updateRequests, BIN));
    // Check Rainmaker updates
    if (updateRequests & FLAG_RM_HUE)  { rmUpdateHue(); }
    if (updateRequests & FLAG_RM_SAT)  { rmUpdateSat(); }
    if (updateRequests & FLAG_RM_DIM)  { rmUpdateDim(); }
    if (updateRequests & FLAG_RM_POW)  { rmUpdatePow(); }

    // Check MQTT updates
    if (updateRequests & FLAG_MQTT_COL) { send_mqttColor(); }
    if (updateRequests & FLAG_MQTT_SW)  { send_mqttSwitch(); }

    updateRequests = 0; // Reset alle vlaggen in één keer na uitvoering
  }
  
  
   checkActionFlag();
   delay(1);
   server.handleClient();
   vTaskDelay(1);
}
