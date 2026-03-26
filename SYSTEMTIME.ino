
void getTijd() 
{
// we try 3 times to retrieve the time
    uint8_t cntr = 1;
    while (!timeRetrieved) {
    // we attempt 3 times
      tijdGet();
      if(timeRetrieved) 
      {
        
        break;
      }
      delay(500);
      cntr ++;
      if(cntr > 3) break;
    }
    consoleOut("get time attempts:" + String(cntr)); 
} 

void tijdGet() 
{

  timeRetrieved = false; // stays false until time is retrieved  
  timeClient.begin();
  //unsigned long epochTime = 0;
  //get the time, if fails we try again during healthcheck

  timeClient.update();
  //Epoch time (also called Unix time) is:
  //The number of seconds that have elapsed since 00:00:00 UTC on January 1st, 1970
  unsigned long epochTime = timeClient.getEpochTime();


      if(epochTime < 1672531200) 
      {  //compare with 1 jan 2023 00:00:00 utc
        ntpUDP.stop();
        UpdateLog(1, "time invalid");
        return;
      }  
    
      epochTime += atoi(settings.gmtOffset) * 60;
      setTime(epochTime); // we have to dit it like this otherwise zomertijd() doesn't work
      dst = 0;
      if ( settings.DTS) 
      {
        // if it is currently summertime we add an hour  
        if ( isSummertime()) 
        {  
          epochTime += 3600; // een uur erbij
          setTime(epochTime);
          dst = 1;
          //DebugPrint("epoch corrected with dts = "); //DebugPrintln(epochTime);
        } else { dst = 2; }
      }
    
    timeRetrieved = true;  
    UpdateLog(1, "got time");
   
    ntpUDP.stop();
    // now if a transition from summertime to winter time or reversed took place
    // we always anticipate on that by recalculating things
    sun_setrise(); // calculate the sunrise and sunset
    mustCalc[0] = mustCalc[1] = mustCalc[2] = mustCalc[3] = true; 
    //}
    //yield();
}

void sun_setrise() {

float OUR_longtitude = settings.longi;
float OUR_latitude = settings.lati;
float OUR_timezone = atof(settings.gmtOffset);  //   120                     // localtime with UTC difference in minutes

sunMoon  sm;

//  tmElements_t  tm;                             // specific time
//  tm.Second = 0;
//  tm.Minute = 12;
//  tm.Hour   = 12;
//  tm.Day    = 3;
//  tm.Month  = 8;
//  tm.Year   = 2016 - 1970;
//  time_t s_date = makeTime(tm);
//  Serial.println("RTC has set the system time");
  sm.init(OUR_timezone, OUR_latitude, OUR_longtitude);


      Sunrise = sm.sunRise();
      Sunset  = sm.sunSet();
      if(settings.DTS)
      {// is time savings applicable 
        if ( isSummertime() )  // summertime or transition winter->summer
        { // it is summertime so we add 1 hr
            Sunrise = Sunrise + 3600; // seconds
            Sunset  = Sunset + 3600;
        }
      } 
 UpdateLog(1, "calc sunset/rise");
 }

// void printDate(time_t date) {
//   char buff[20];
//   sprintf(buff, "%2d-%02d-%4d %02d:%02d:%02d",
//   day(date), month(date), year(date), hour(date), minute(date), second(date));
//   Serial.print(buff);
// }

// *****************************************************************
//                    berekeningen timers van 0 t/m ...
//******************************************************************
void timerCalc(int nummer) 
{  
  // first check if the calculation is needed.
  // when not we return with mustswich false and mustcalc false
  // mutswitch can only be true if weekDag = y  and there are values in sw_on and sw_off and timer[nummer]=true 
  // but otherwise we wouldn't be here
  // timer[nummer] = should be true otherwise we wouldn't be here

  // weekDay() is the weekday now (Sunday is day 1)
  // in the bool dow sunday starts at [0] so we should check -1

  //the array of bools starts at 0 so if we have wensday (true in [3])
  //there is a but this would mean that sat = day(7)
  //sunday true[0] sat true in[6] 
  // when the timer = active, the day is true and it is not already done
  if(timers[nummer].dow[weekday() -1 ]  == true && timers[nummer].Active == true && mustCalc[nummer])
    {
      consoleOut("mustSwitch set true");
    } else {
      mustSwitch[nummer] = false; 
      mustCalc[nummer] = false;
      consoleOut("mustSwitch and mustcalc are false");
      return;
    }

    switchOnTime[nummer] = calculateSwitchTime(
      timers[nummer].on_hh,
      timers[nummer].on_mm,
      timers[nummer].on_mode
      );
    
    switchOfTime[nummer] = calculateSwitchTime(
      timers[nummer].of_hh,
      timers[nummer].of_mm,
      timers[nummer].of_mode
      );

    if(switchOfTime[nummer] < switchOnTime[nummer]) switchOfTime[nummer] += 24 * 3600;; // we add 24 hrs 
    Serial.print("now = " );
    time_t nu = now();
    Serial.println(ctime(&nu));
    if(now() > switchOfTime[nummer])  {
        Serial.println("timer+ + String(mummer) + has expired");
        mustSwitch[nummer] = hasSwitched[nummer] = false; //// this timer has expired so
    }
    consoleOut("the switch times for timer " + String(nummer));
    consoleOut("Switch ON at: ");
    consoleOut(ctime(&switchOnTime[nummer]));
    consoleOut("Switch OFF at: ");
    consoleOut(ctime(&switchOfTime[nummer]));
 
    mustSwitch[nummer] = true;
    mustCalc[nummer] = false;
    String term = "calculated timer " + String(nummer);
    UpdateLog(1 , term ); // system calculated timer 1
// these calculations are correct
}


// this function calculates the switchtime as a time_t value

time_t calculateSwitchTime(uint8_t hh, uint8_t mm, uint8_t mode) 
{
    time_t now = time(NULL);
    struct tm tmNow;
    localtime_r(&now, &tmNow);

    int offsetSeconds = (hh * 60 + mm) * 60;
    
    switch (mode) {

        case MODE_ABSOLUTE: {
            tmElements_t tm;
            tm.Hour = hh;
            tm.Minute = mm;
            tm.Second = 0;
            tm.Day = day();
            tm.Month = month();
            tm.Year = year() - 1970;
            return makeTime(tm);
        }

        case MODE_BEFORE_SUNRISE:
            return Sunrise - offsetSeconds;

        case MODE_AFTER_SUNRISE:
            return Sunrise + offsetSeconds;

        case MODE_BEFORE_SUNSET:
            return Sunset - offsetSeconds;

        case MODE_AFTER_SUNSET:
            return Sunset + offsetSeconds;
    }

    return 0; // invalid mode
}


// void timeConvert () {
// asouur = tijd_cvrt( aso, true );
// asominuut = tijd_cvrt( aso, false );
// }


// // een universele funtie om het aantal uren en minuten uit de char in een string te zetten
// // als wat true dan geeft ie de uren en anders de minuut
// int tijd_cvrt(char TIJD[6], bool wat) {
//      int uur = 0;
//      for (int i = 0; i < 2; i++) {
//           char c = TIJD[i];
//                if (c < '0' || c > '9') break;
//                uur *= 10;
//                uur += (c - '0');
//           }
//      int minuut = 0;
//      for (int i = 3; i < 5; i++) { // 2 = de :
//           char c = TIJD[i];
//                if (c < '0' || c > '9') break;
//                minuut *= 10;
//                minuut += (c - '0');
//           }
// if (wat) { return uur; } else { return minuut;}
// }


// new way to detect sumer or wintertime
bool isSummertime() 
{
  time_t tNow = now();
  int y = year(tNow);
  // we calculate the time at Last Sunday of March
  tmElements_t tm;
  tm.Year = y - 1970;
  tm.Month = 3;
  tm.Day = 31;
  tm.Hour = 2;
  tm.Minute = 0;
  tm.Second = 0;
  
  time_t t = makeTime(tm);
  //maketime(tm) = last day in oct 31
  //say weekday() of this time = 4 (example it is an wednesday)
  // then the last sunday = (4-1) * SECS_PER_DAY earlyer.
  uint8_t subtract = weekday(t)-1;
  time_t start = t - subtract * SECS_PER_DAY;

  // we calculate the time at Last Sunday of October
  tm.Month = 10;
  tm.Day = 31;
  tm.Hour = 3;
  t = makeTime(tm);
  subtract = weekday(t)-1;
  time_t end = t - subtract * SECS_PER_DAY;

  // --- basic summer/winter determination ---
  //if (tNow >= start && tNow < end) return true;   // 1 = summer, 0 = winter
  return (t >= start && t < end); // true if t is between start and end
  return false;

}