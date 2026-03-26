
void read_onboard_button() {
    if (digitalRead(onboard_button) == LOW) {  //Push button pressed

        // Key debounce handling
        delay(100);
        int startTime = millis();
        while (digitalRead(onboard_button) == LOW) {
          delay(50);
        }
        int endTime = millis();

        if ((endTime - startTime) > 10000) {
          // If key pressed for more than 10secs, reset all
          Serial.printf("Reset to factory.\n");
          RMakerFactoryReset(2);
        } else if ((endTime - startTime) > 3000) {
          Serial.printf("Reset Wi-Fi.\n");
          // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
          RMakerWiFiReset(2);
        } else {
          // Toggle device state
          strip_onoff = !strip_onoff;
          Serial.printf("Toggle State to %s.\n", strip_onoff ? "true" : "false");
          // if (my_device) {
          //   my_device->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, dimmer_state);
          // }
          //(strip_state==true) ? set_dim_level(last_duty): set_dim_level(0);
        }
      }

}


// void read_gpio_button() {
//     static bool last_btn = HIGH;
//     static unsigned long last_dim_time = 0;

//     bool btn = digitalRead(gpio_button);

//     // Button pressed
//     if (btn == LOW && last_btn == HIGH) {
//         press_start = millis();
//         long_press_active = false;
//     }

//     // Button held
//     if (btn == LOW) {
//         if (!long_press_active && millis() - press_start > LONG_PRESS_TIME) {
//             long_press_active = true;
//             dim_up = !dim_up;  // toggle direction at hold start
//         }
//     //whenever the led is on, it is dimmed at target_duty so this is our start
        
//         if (long_press_active && dimmer_state) {
//             while(digitalRead(gpio_button) == LOW) {
//               // as long as the button is low we dim up / down with 140
//               (dim_up == false) ? target_duty -= 140 : target_duty += 140;
//               if(target_duty > 8192) target_duty = 8192; break;
//               if(target_duty < 100) target_duty = 100; break;
//               set_pwm(target_duty);
//               delay(80);
//               } 
//         }
//     }

//     // Button released
//     if (btn == HIGH && last_btn == LOW) {
//         if (!long_press_active) {
//             dimmer_state = !dimmer_state; // toggle true and false
//             set_power(dimmer_state);  // handles fade in/out and restores last_dim_level
//         }
//     }

//     last_btn = btn;
// }


void check_button1()
{
  //bool bts;
  // first debounce by checking a few millis later if it's still high
  //bts=digitalRead(button1);
  //if(bts == LOW ) delay(deBounce);  
  if (digitalRead(button1) == LOW) {
  delay(50);
  // now we read again
  if(digitalRead(button1) == HIGH) {
    //glitchCounter += 1; // this counter checks if there are spikes on
    Serial.println("check_button1: spike!"); 
    return; 
    }
  
  //so the button is still low, now we wait a little
  //to see if it it stil low. If not, the button
  // has been clicked, so we switch
  delay(300);
  if(digitalRead(button1) == HIGH){ // released
      strip_onoff = !strip_onoff; // toggle true and false
      Serial.printf("Toggle State to %s.\n", strip_onoff ? "true" : "false");
      //set_power(dimmer_state); 
      delay(100); // prevent re-reading before ready fading
      //(dimmer_state == true) ? UpdateLog(6, "switched on") : UpdateLog(6, "switched off");
      if(strip_onoff == true, true) 
      { 
          UpdateLog(6, "switched on"); 
          setStripOn();
      } else 
      { 
        UpdateLog(6, "switched off");
        stripOff(1);
        //set_dim_level(0);
      }
      //update_strip(true, true);
  } 
      else 
  {
      // the button is still low the dim fuction is called
      dim_up = !dim_up; // toggle the direction
      // when the duty is already max, we dim down by default and reverse
      if(strip_level >= 100) dim_up = false;
      if(strip_level <= 1) dim_up = true;
      Serial.printf("Toggle dim direction to %s.\n", dim_up ? "true" : "false");
      while(digitalRead(button1) == LOW) {
        // as long as the button is low we dim +/- with steps of 140
        (dim_up==true) ? strip_level += 5 : strip_level -= 5;
        
        if(strip_level > 100) strip_level = 100;
        if(strip_level < 1) strip_level = 1; // cannot become 0
        //Serial.println(String(current_level));
        setDim(1);
        delay(100);
        }     

        // now we should inform rainmaker about this change ( stae and level)
        if (my_device) {
               my_device->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, strip_state);
               my_device->updateAndReportParam(ESP_RMAKER_DEF_BRIGHTNESS_NAME, (int)strip_level);
        
        }
        UpdateLog(6, "dim command");

       }
   }
}
