
const char TIMERCONFIG_MAIN[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta charset='utf-8'>
<title>ESP32-C3-DIMMER</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" type="text/css" href="/STYLES">
<style>
iframe { width: 100%; height: 570px; border: 1px solid #ccc; margin-top: 10px; }
</style>
</head>
<body>
<div class="container">
    <div class="nav">
        <a onclick="loadTimer(0)">timer 0</a>
        <a onclick="loadTimer(1)">timer 1</a>
        <a onclick="loadTimer(2)">timer 2</a>
        <a onclick="loadTimer(3)">timer 3</a>
        <span class='close'><a href="/MENU">X</a></span><br>   
</div>

    <!-- Hierin wordt de content geladen -->
    <iframe id="contentFrame" src="/timer?welke=%welketimer%"></iframe>

    <script>
        function loadTimer(id) {
            document.getElementById('contentFrame').src = "/timer?welke=" + id;
        }
    </script>
</body>
</html>
 )=====";
                                                                        
 const char TIMER_GENERAL[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<link rel="stylesheet" type="text/css" href="/STYLES">

<style>
body {font-family:arial; font-size:14px;}
tr {height: 40px;}
.clean-table th {
  background: none;    
}
table {width: 90%; table-layout: fixed;}
input[type='time'], select.sb1 {
    width: 100%;          
    box-sizing: border-box; 
    padding: 5px;
}
</style>

</head><body>
<center><br><H1 style='color:#e207fa;'>TIMER {nr} </h1>
<form id='formulier' method='get' action='/submitTimers' target='_parent'>

<center><table>
<tr><td style='width:60px;'>active?<td style='width:100px'><input type='checkbox' name='ta' tActive>

<tr><td>scene<td><input class ='inp3' type='number' name='pro' value="{pro}" min="1" max="5"><td style='width:120px'></td></tr>
<tr><td>level<td><input class ='inp3' type='number' name='lev' value="{lev}" min="10" max="100"><td style='width:120px'></td></tr>
<tr>
<tr><td>on :
<td><input class='inp4' type='time' name='inw' value='{onX}' title='hh:mm'>
<td><select name='zonattaan' class='sb1'>
<option value='0' zonattaan_0>absolute</option>
<option value='1' zonattaan_1>before sunrise</option>
<option value='2' zonattaan_2>after sunrise</option>
<option value='3' zonattaan_3>before sunset</option>
<option value='4' zonattaan_4>after sunset</option>

</select></td></tr>
<tr><td>off :<td><input class='inp4' name='uitw' type='time' value='{ofX}' title='hh:mm' >
<td><select name='zonattuit' class='sb1'>
<option value='0' zonattuit_0>absolute</option>
<option value='1' zonattuit_1>before sunrise</option>
<option value='2' zonattuit_2>after sunrise</option>
<option value='3' zonattuit_3>before sunset</option>
<option value='4' zonattuit_4>after sunset</option>
</select></td></tr></table>

<br><table class="clean-table">
<tr><thead>
<th> su <input type='checkbox' name='su' selzo></th>
<th> mo <input type='checkbox' name='mo' selma></th>
<th> tu <input type='checkbox' name='tu' seldi></th>
<th> we <input type='checkbox' name='we' selwo></th>
<th> th <input type='checkbox' name='th' seldo></th>
<th> fr <input type='checkbox' name='fr' selvr></th>
<th> sa <input type='checkbox' name='sa' selza></th>
</tr></thead></table>
<br>
<button type="submit" class="btn">SAVE TIMER </button>
</center><br></html>
 )=====";

