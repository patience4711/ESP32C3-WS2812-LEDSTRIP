// find out if the request comes from inside the network
bool checkRemote(String url) {
//check if the first 9 characters of the router 192.168.0
    if(settings.securityLevel == 0) return false; // never remote
    if ( url.indexOf(WiFi.gatewayIP().toString().substring(0, settings.securityLevel)) == -1 ) return true; else return false;
}
// when remote true we deny the switching
// we come here when an unknown request is done
void handleNotFound() {
  
bool intern = false;
if(!checkRemote(server.client().remoteIP().toString()) ) intern = true;

// **************************************************************************
//             R E S T R I C T E D   A R E A
// **************************************************************************
// access only from inside the network
if ( intern ) {    //DebugPrintln("the request comes from inside the network");
     String serverUrl = server.uri().c_str();
     consoleOut("\nnotfound serverUrl = " + serverUrl);             

     if(serverUrl == "/SW=ON") {
      consoleOut("sw=on found");
      if(strip_onoff == false) {
         strip_onoff = true;
         if(settings.scene != 0) setScene(); else setStripOn();
         UpdateLog(5, "switched on");
         server.send ( 200, "text/plain", "switched on" );
      } else {
         server.send ( 200, "text/plain", "already on, skip" );
      }
      return; 
     }
     if(serverUrl == "/SW=OFF") {
      consoleOut("sw=off found");
      if(strip_onoff == true) {
         strip_onoff = false;
         stripOff(1);
         //update_strip(true, true);
         //checkTimers();
         UpdateLog(5, "switched off");
         server.send ( 200, "text/plain", "switched off" );
      } else {
         server.send ( 200, "text/plain", "already off, skip" );
      }
      return; 
     }
     // if we are here, no valid api was found    
     server.send ( 200, "text/plain", "ERROR this link is invalid, go back <--" );//send not found message
     }             
    else 
     { // not intern
      //DebugPrint("\t\t\t\t unauthorized, not from inside the network : ");
      server.send ( 200, "text/plain", "ERROR you are not authorised, go back <--" );//send not found message
     }
}