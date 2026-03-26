void zendpageTimers() {
consoleOut("we are at zendPageTimers");

//String toSend = FPSTR(HTML_HEAD);
String toSend = FPSTR(TIMERCONFIG_MAIN);
// we should restore the last timerpage that we used
toSend.replace("%welketimer%", String(tKeuze)); // this shouild lead to the last edited timer
toSend.replace("tieTel", settings.dvName );

server.send(200, "text/html", toSend);


}

// ****************************************************************************
//             we compose a page that is exposed in the frame
// ****************************************************************************
void handleTimer() {
  String toSend = FPSTR(TIMER_GENERAL);
  // 1. get tKeuze from the  URL (bex. /timer?tkeuze=2)
  int id = 0;
  if (server.hasArg("welke")) {
    tKeuze = server.arg("welke").toInt();
  }
  
  consoleOut("we are at handleTimer, tkeuze = " + String(tKeuze));
  
  toSend.replace("{nr}" , String(tKeuze)); // vervang timer nummer
        
    if(timers[tKeuze].Active) toSend.replace("tActive", "checked");
    // we put back "selected" for the option in the selectbox zonattaanwelke_1 2 3 4 or 5 
    toSend.replace("{lev}" , String(timers[tKeuze].Level)); // vervang level
    //toSend.replace("{hue}" , String(timers[tKeuze].Hue)); // vervang hue
    toSend.replace("{pro}" , String(timers[tKeuze].Pro)); // vervang pro
    toSend.replace(zonatt_replace(String(timers[tKeuze].on_mode), "zonattaan"), "selected");
    toSend.replace(zonatt_replace(String(timers[tKeuze].of_mode), "zonattuit"), "selected");
    
     // put back the checked selectboxes 
    const char *grap[] = {"selzo", "selma", "seldi", "selwo", "seldo", "selvr", "selza"};
    String vervang = "";
    //weekDag
     consoleOut("replace checkboxes to show the checked ones");
    // voor deze timer doen, 7x
    //int i = tKeuze;
        for(int x=0; x<7; x++){ // bij 3 is dit van 21 tot 27 kleiner dan 28

          vervang = String(grap[x]); // als i=3 dan van 21-21 naar 27

               if (timers[tKeuze].dow[x] == true) { toSend.replace(vervang, "checked");}
         }
    
    
      consoleOut("replace the time values");
      //toSend.replace("{inx}"  , "" + String(switchOn[tKeuze*5]) + String(switchOn[tKeuze*5+1]) + String(switchOn[tKeuze*5+2]) + String(switchOn[tKeuze*5+3]) + String(switchOn[tKeuze*5+4]));
      //toSend.replace("{uitx}" , "" + String(switchOff[tKeuze*5]) + String(switchOff[tKeuze*5+1]) + String(switchOff[tKeuze*5+2]) + String(switchOff[tKeuze*5+3]) + String(switchOff[tKeuze*5+4])); 
      char buf[6];  // "HH:MM\0"
      snprintf(buf, sizeof(buf), "%02u:%02u", timers[tKeuze].on_hh, timers[tKeuze].on_mm);
      toSend.replace("{onX}"  , buf );
      snprintf(buf, sizeof(buf), "%02u:%02u", timers[tKeuze].of_hh, timers[tKeuze].of_mm);
      toSend.replace("{ofX}"  , buf);

      server.send(200, "text/html", toSend);
}

void handleTimerSave()
{
// collect the formdata
          //strip_state = 0; // prevent slow down
          timers[tKeuze].Active = server.hasArg("ta"); // true or false
          //if(server.hasArg("ta"))  timers[tKeuze].Active = true;  else timers[tKeuze].Active = false;
          timers[tKeuze].Level = server.arg("lev").toInt();
          //timers[tKeuze].Hue = server.arg("hue").toInt();
          timers[tKeuze].Pro = server.arg("pro").toInt();
         if (server.hasArg("inw")) {
            String t = server.arg("inw");  // "18:30"
            timers[tKeuze].on_hh = t.substring(0, 2).toInt();
            timers[tKeuze].on_mm = t.substring(3, 5).toInt();
         }
         if (server.hasArg("uitw")) {
            String t = server.arg("uitw");  // "18:30"
            timers[tKeuze].of_hh = t.substring(0, 2).toInt();
            timers[tKeuze].of_mm = t.substring(3, 5).toInt();
         }
         // get the value of the selects
                  
         timers[tKeuze].on_mode = server.arg("zonattaan").toInt(); 
         timers[tKeuze].of_mode = server.arg("zonattuit").toInt();
     
          // put the values of the days in an array
          const char *grap[] = {"su", "mo", "tu", "we", "th", "fr", "sa"};
          //String wd = ""; 
          for (int x = 0; x < 7; x++) {
          // the checkbox only has a parameter when checked
          timers[tKeuze].dow[x] = server.hasArg(grap[x]); // true or false
          //if(request->hasParam(grap[x])) timerProp[tKeuze].dow[x] = true; else timerProp[tKeuze].dow[x] = false;  
          }
           // these functions work ok
          consoleOut("timers[tKeuze].on_mode = " + String(timers[tKeuze].on_mode));
          consoleOut("timers[tKeuze].of_mode = " + String(timers[tKeuze].on_mode)) ;
           
          
           // now send the confirm 
          procesId = 2;
          confirm("/TIMERCONFIG"); // call the confirmpage
          saveTimers();
          mustSwitch[0] = hasSwitched[0] = false;
          strip_onoff = false; // we put the strip off
          stripOff(1);
          mustCalc[tKeuze] = true; // makes it recalc in the loop
          return;
}


// this function returns the value of vervang
// input is p.e. "1" and zonattaan
// returns vervang = zonattaan_1
String zonatt_replace( String argument1, String argument2) 
{
    //String vervang = "";
    String vervang = argument2 + "_" + argument1; // eg zonattaan_1
    consoleOut("zonat_replace returns" + vervang);

return vervang;
}


// ********************************************************************************************
// *                   switching by the timers                                               *
// ********************************************************************************************
// this function filters out for which switch we must evaluate the switch on/off time
// the criteria are musSwitch true, an all other timers hasSwitched false 
// say timer 0 goes out and timer 1 should switch on at an earlier time. 
// we make them wait util the 1st one finished
void schakelen() 
{
    // first find out if any switch is on, if not whoSwitched=255
    uint8_t whoSwitched = 255;
    for(uint8_t x=0; x<4; x++)
      {
        if(hasSwitched[x]) { whoSwitched = x; break; }
      }
    //so now we know who has switched (if any)
    for (uint8_t z = 0; z<4; z++)
    {
       if( !mustSwitch[z]) continue;
       // if we are here mustSwitch[x] = true
       // so is some switched and it is not z we continue
       if( whoSwitched != 255 && whoSwitched != z) continue;
       //if we are here, mustSwitch[z]=true and no other swich hasSwiched
        timer_schakel_in(z);
        timer_schakel_uit(z);
     }
}



// check if it is time to switch
void timer_schakel_in(int welke) {
            if ( now() > (switchOnTime[welke]) && now() < switchOfTime[welke] && !hasSwitched[welke]) { 
                lampOnNow(welke+20); // lamp on right away
                hasSwitched[welke] = true;
                consoleOut("switched on by timer" + String(welke));
            }
}
 
void timer_schakel_uit(int welke) {
         if ( now() > switchOfTime[welke] && hasSwitched[welke] ) { // als event 3 4 5 of 6 is
              lampOffNow(welke+20); //lamp off right away, mqtt message, checkTimers
              mustSwitch[welke] = false;
              hasSwitched[welke] = false; // prevent repetitions
              consoleOut("switched off by timer"+ String(welke));
             }
    }

void checkTimers() {
      // if switched off manual this should prevent that a timer puts it on again
      // so we disarm a timer that is active (hasSwitched=true) at that moment 
      if ( hasSwitched[0] ) mustSwitch[0] = hasSwitched[0] = false; //hasSwitched[0]=false; // to prevent that its switches on again
      if ( hasSwitched[1] ) mustSwitch[1] = hasSwitched[1] = false;// to prevent that its switches on again
      if ( hasSwitched[2] ) mustSwitch[2] = hasSwitched[2] = false;// to prevent that its switches on again
      if ( hasSwitched[3] ) mustSwitch[3] = hasSwitched[3] = false;// to prevent that its switches on again 
}

void lampOnNow(int who) {
      if(who > 19) // if switched by a timer we should fade to timers[wie].Level
      {   
        uint8_t wie = who-20;
        strip_level = timers[wie].Level;
        consoleOut("strip_level set to " + String(strip_level));
        //strip_hue = timers[wie].Hue;
        //consoleOut("strip_hue set to " + String(strip_hue));
        //strip_sat = timers[wie].Sat;
        //consoleOut("strip_sat set to " + String(strip_sat));
        strip_state = timers[wie].Pro;
        strip_onoff = true;
        setStripOn();
      }
  }





void lampOffNow(int who) 
    {
        strip_onoff = false;
        consoleOut("switch off");
        UpdateLog(who, "switched off");
        stripOff(1);
    }
