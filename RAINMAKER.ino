
static void mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == RMAKER_COMMON_EVENT) {
        switch (event_id) {
            case RMAKER_MQTT_EVENT_CONNECTED:
                Serial.println("\nMQTT Connected.");
                rm_associated = true;
                break;
            case RMAKER_MQTT_EVENT_DISCONNECTED:
                Serial.println("\nMQTT Disconnected.");
                break;
            default:
                Serial.println("\nUnhandled RainMaker Event:");
        }
    }
}

// WARNING: sysProvEvent is called from a separate FreeRTOS task (thread)!
void sysProvEvent(arduino_event_t *sys_event) {
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      WiFiProv.printQR(service_name, pop, "ble");
      break;

    case ARDUINO_EVENT_PROV_INIT:         
      WiFiProv.disableAutoStop(10000); 
      break;

    case ARDUINO_EVENT_PROV_CRED_SUCCESS: 
      // Credentials zijn binnen, maar we wachten op IP om Bluetooth te killen
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("\nWiFi connected! got IP. Shutoff Bluetooth ...");
      WiFiProv.endProvision(); 
      
      // Nu komt er een flinke brok RAM vrij voor de SSL handshake van RainMaker
      break;

    default:
      break;
  }
}

void write_callback(Device *device, Param *param,
                    const param_val_t val, void *priv_data,
                    write_ctx_t *ctx)
{
    const char *param_name = param->getParamName();
    const char *device_name = device->getDeviceName();
    //Serial.printf("PARAM %s\n", param->getParamName());

    if (strcmp(param_name, "Brightness") == 0) {
        strip_level = val.val.i; // 0–100 
        Serial.printf("\nLevel cmd, value = %d for %s - %s\n", val.val.i, device_name, param_name); 
        //if (dimmer_state) // ha mod { 
        //int duty = map(level, 0, 100, 0, 255); // ha mod //
        //update_leds(level); 
        UpdateLog(4, "dim command");
    }
    else if (strcmp(param_name, "Power") == 0) {
        Serial.printf("Power cmd, value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
        strip_onoff = val.val.b; //
        if(strip_onoff == false)
         {
          //set_dim_level(0);
          UpdateLog(4, "switched off");
          // disarm a timer when active
          checkTimers();
         } else {
          //set_dim_level(last_duty);
          UpdateLog(4, "switched on");
         }
        //param->updateAndReport(val);
    
         } else if (strcmp(param_name, ESP_RMAKER_DEF_HUE_NAME) == 0) {
              strip_hue = val.val.i;
              strip_state = 4; //makes the strip go on
              strip_onoff = true; // otherwise it just switches off
         } else if (strcmp(param_name, ESP_RMAKER_DEF_SATURATION_NAME) == 0) {
              strip_sat = val.val.i;
              strip_state = 4; //makes the strip go on
              strip_onoff = true; // otherwise it just switches off
         }

    update_strip(); 

}

