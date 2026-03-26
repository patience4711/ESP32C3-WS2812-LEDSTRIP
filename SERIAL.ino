void handle_Serial () {
    Serial.println("\n* * * handle_Serial, type LIST-COMMANDS");  
    int SerialInByteCounter = 0;
    char InputBuffer_Serial[256] = "";
    byte SerialInByte;  
    // first check if there is enough data, at least 13 bytes
    delay(200); //we wait a little for more data as the esp seems slow
    if(Serial.available() < 13 ) {
      // less then 13, we can't expect more so we give up 
      while(Serial.available()) { Serial.read(); } // make the buffer empty 
      Serial.println("invalid command, abort " + String(InputBuffer_Serial));
     return;
    }

// now we know there are at least 13 bytes so we read them
 //diagNose = true; //direct output to serial
 while(Serial.available()) {
             SerialInByte=Serial.read(); 
             //Serial.print("+");
            
            if(isprint(SerialInByte)) {
              if(SerialInByteCounter<130) InputBuffer_Serial[SerialInByteCounter++]=SerialInByte;
            }    
            if(SerialInByte=='\n') {                                              // new line character
              InputBuffer_Serial[SerialInByteCounter]=0;
              //   Serial.println(F("found new line"));
             break; // serieel data is complete
            }
       }   
   Serial.println("InputBuffer_Serial = " + String(InputBuffer_Serial) );
   int len = strlen(InputBuffer_Serial);
   // evaluate the incomming data

     //if (strlen(InputBuffer_Serial) > 6) {                                // need to see minimal 8 characters on the serial port
     //  if (strncmp (InputBuffer_Serial,"10;",3) == 0) {                 // Command from Master to RFLink
  
          if (strcasecmp(InputBuffer_Serial, "LIST-COMMANDS") == 0) {
              Serial.println(F("*** AVAILABLE COMMANDS ***"));
              Serial.println(F("RESET_PASSWD (reset admin passwd to 0000)"));
              Serial.println(F("PRINTOUT-PREFS"));
              Serial.println(F("DEVICE-REBOOT"));
              return;
    } else 

   //8 41.40	25.51	15.98	25.90	5.89
  //9 41.06	30.33	9.97	1644.36	38.48	2.74	4.62	10.64.13	6.10
  // 10 44.36	38.48	2.74	4.62	10.64
  // 11 48.30	47.99	0.77	1.25	50.94
  // 12	94.45	101.43	0.42	0.17	155.28
   // 

   
   if (strncasecmp(InputBuffer_Serial, "RESET-PASSWD" , 12) == 0) 
   { 
        // the buffer could be something like WRITE-MONTH={"CON_LT":12.34, "CON_HT":56.567, etc}
        Serial.println("resetting the admin passwd to 0000");
        strcpy(settings.passwd, "0000");
        saveSettings(); 
       return;
    } else   
   
   
    if (strcasecmp(InputBuffer_Serial, "PRINTOUT-PREFS") == 0) 
    {  
          Serial.println("--- Current Settings ---");
          Serial.printf("Device Name   : %s\n", settings.dvName);
          Serial.printf("Password      : %s | UserPwd: %s\n", settings.passwd, settings.userPwd);
          Serial.printf("Coords (Lat)  : %.3f | (Long): %.3f\n", settings.lati, settings.longi);
          Serial.printf("Time          : GMT %s | DST: %s\n", settings.gmtOffset, settings.DTS ? "Yes" : "No");
          Serial.printf("Security      : %u | numpix: %d%%\n", settings.securityLevel, settings.numpix);
          Serial.printf("MQTT Broker   : %s:%d (Format: %d)\n", settings.Mqtt_Broker, settings.Mqtt_Port, settings.Mqtt_Format);
          Serial.printf("MQTT In/Out   : %s / %s\n", settings.Mqtt_inTopic, settings.Mqtt_outTopic);
          Serial.printf("MQTT Auth     : User: %s | ClientID: %s\n", settings.Mqtt_Username, settings.Mqtt_Clientid);
          Serial.printf("Diagnose      : %s\n", settings.diagNose ? "Active" : "Disabled");
         
          Serial.println("--- Timer Scheme ---");
          for (int i = 0; i < 4; i++) 
          {
            Serial.printf("Timer [%d] - %s | On: %02d:%02d (Mode: %u) | Off: %02d:%02d (Mode: %u) | Lvl: %d\n", 
              i, 
              timers[i].Active ? "ACTIEF" : "UIT   ",
              timers[i].on_hh, timers[i].on_mm, timers[i].on_mode,
              timers[i].of_hh, timers[i].of_mm, timers[i].of_mode,
              timers[i].Level
            );
            
            // Optioneel: de Dagen van de Week (dow) printen
            Serial.print("   Days: ");
            const char* dagen[] = {"mo", "tu", "we", "th", "fr", "sa", "so"};
            for (int d = 0; d < 7; d++) {
              if (timers[i].dow[d]) Serial.printf("%s ", dagen[d]);
            }
            Serial.println("\n--------------------");
  
            }
        
        return;
    } else

    if (strcasecmp(InputBuffer_Serial, "DEVICE-REBOOT") == 0) {
           Serial.println("\ngoing to reboot ! \n");
           delay(1000);
           ESP.restart();
    } else {

          
      Serial.println( String(InputBuffer_Serial) + " INVALID COMMAND" );     
     }

       
    // } // end if if (strncmp (InputBuffer_Serial,"10;",3) == 0)
    Serial.println( String(InputBuffer_Serial) + " UNKNOWN COMMAND" );
    //  end if strlen(InputBuffer_Serial) > 6
  // the buffercontent is not making sense so we empty the buffer
  empty_serial();
   //
} 


void empty_serial() { // remove any remaining data in serial buffer
  while(Serial.available()) {
    Serial.read();
    //uart_read_bytes(UART_NUM_2, &data, 1, 20 / portTICK_PERIOD_MS);
  }
}