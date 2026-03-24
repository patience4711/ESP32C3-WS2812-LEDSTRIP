//<link rel="stylesheet" type="text/css" href="/STYLESHEET">

const char home_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset='utf-8' name="viewport" content="width=device-width, initial-scale=1"><title>ESP32C3 LEDSTRIP</title>
<link rel="stylesheet" type="text/css" href="STYLES">  
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
  <style type='text/css'>
  .slidecontainer {  width: 90%;}
  .slider {  -webkit-appearance: none;  width: 94%;  height: 28px;  background: linear-gradient(to left, white, black);  border: solid 1px;  border-radius:10px;  outline: none;  opacity: 1.0;  -webkit-transition: .2s;  transition: opacity .2s;}
  .slider:hover {  opacity: 1;}
  .slider::-webkit-slider-thumb {  -webkit-appearance: none;  appearance: none;  width: 25px;  height: 25px;  background: white;  border:solid 1px;  border-radius:10px;  cursor: pointer;}
  .slider::-moz-range-thumb {  width: 25px;  height: 25px;  background: white;  border:solid 1px; cursor: pointer;}
  .BSat{background: linear-gradient(to left, grey, white); }
  .BHue{background: linear-gradient(to right, #ff0000 0%, #ffff00 17%, #00ff00 33%, #00ffff 50%, #0000ff 67%, #ff00ff 83%, #ff0000 100%);}  /* De regenboog gradient: Rood -> Geel -> Groen -> Blauw -> Paars -> Rood */
  
  .BDim{background: linear-gradient(to left, white, black); }
  .bt_on{ background:green; color:black;}
  .bt_on::before{content:"RESUME";}
.bt, .bt2{
    display:block;
    width:124px;
    height:44px;
    padding:6px;
    margin:6px;
    text-align:center;
    border-radius:10px;
    color:white;
    background:#b9b9c1;
    font-weight:bold;
    font-size:18px;
    text-decoration:none;
    box-shadow: 0 6px #999; 
    }

  .bt:active {
    background-color: #3e8e41;
    box-shadow: 0 2px #666;
    transform: translateY(4px);
  }  
//    .red{
//     background:#b9b9c1;
//     color:white;
//     border:2px solid black;
//     }

  .slidecontainer label {
    font-size: 12px;
    color: black;
    display: block;
  }

@media only screen and (max-width: 800px) { 
.bt {width: 80px; font-size: 12px;}
}


</style> 
</head>
<body onload='loadScript()'>
<div class='container' id='maindiv'><center>
<div class='nav'>
<a href="/MENU" style="float:right">menu</a>
<a href="#" id="sub" style='background:green; display: none' onclick='submitFunction()'>save</a><br>
</div>
<br><br>
<h2>ESP32C3 LEDSTRIP CONTROL <span id="NAME"></span></h2><br><br>

<b id="pwdby">powered by Hansiart</b><br>
<br><br>
<table>
  <tr><td><button id='bt0' onclick='buttonFunction(0)' class='bt'>OFF</button></td>
  <td><button id='bt1' onclick='buttonFunction(1)' class='bt'>WARM</button></td>
  <td><button id='bt2' onclick='buttonFunction(2)' class='bt'>NEUTRAL</button></td>  
  <tr><td><button id='bt3' onclick='buttonFunction(3)' class='bt'>COOL</button></td>
  <td><button id='bt4' onclick='buttonFunction(4)' class='bt'>COLOR</button></td>
  <td><button id='bt5' onclick='buttonFunction(5)' class='bt'>HUE FLOW</button></td>
  </table>
  <br><br>
  <div class='slidecontainer'>
  <label>Color tone (Hue): <span id="val-hue">180</span>°</label>
  <input type="range" id="hue" name="hue" min="0" max="359" class="slider BHue" value="{HUE}" oninput="updateLabel('hue', this.value)" onchange='sliderFunction(1, this.value)'>
  </div><br>
  
<div class='slidecontainer'>
  <label>Saturation: <span id="val-sat">50</span>%</label>
  <input type='range' id='sat' name="sat" max='99' min='1' class='slider BSat' value='{SAT}' oninput="updateLabel('sat', this.value)" onchange='sliderFunction(2, this.value)'>
  </div><br>

  <div class='slidecontainer'> 
   <label>Brightness: <span id="val-level">50</span>%</label>
  <input type="range" id="level" name="level" min="0" max="100" class="slider BDim" value="{Dim}" oninput="updateLabel('level', this.value)" onchange='sliderFunction(3, this.value)'>
  </div><br>
  </div>
</div>

<script defer>
//var myLink=document.getElementById("goBack");
// Wanneer de gebruiker terugkeert naar de tab/pagina
document.addEventListener('visibilitychange', () => {
    if (document.visibilityState === 'visible') {
        console.log("Tab weer actief, data ophalen...");
        updateUI();
    }
});

//const btn = document.getElementById('bt4');
//var lampwaarde = 0;

function loadScript() {
  updateUI();
}

function updateUI() {
    fetch('/get.Data')
        .then(r => r.json())
        .then(data => {
            applyDataToUI(data);
            // Plan de volgende update pas in NADAT deze klaar is
            setTimeout(updateUI, 3000); 
        })
        .catch(err => {
            console.error("Status fout:", err);
            // Bij een fout (bijv. timeout), probeer het over 5 seconden opnieuw
            setTimeout(updateUI, 5000);
        });
}

function buttonFunction(index) {
    // We sturen de data als JSON naar je server endpoint
    fetch('/buttons?button=' + index, {  
        method: 'POST',
    })
    .then(response => response.text())
    .then(data => console.log('Succes:', data))
    .catch(error => console.log('Fout:', error));
}

function sliderFunction(index, waarde) {
    // We send the index and the value
    fetch(`/sliders?slider=${index}&val=${waarde}`)
        .then(response => response.text())
        .then(data => console.log("Server says:", data));
}

function applyDataToUI(data) {
    // 1. De Power knop (onoff)
    const pwrBtn = document.getElementById('bt0');
    const maindiv = document.getElementById('maindiv');
    //onst satSlider = document.getElementById('sat');
    if (pwrBtn) {
        if (data.onoff === 1) {
            pwrBtn.style.backgroundColor = "#b9b9c1"; 
            maindiv.style.backgroundColor = "white";
        } else {
            pwrBtn.style.backgroundColor = "#1f8a07";
            maindiv.style.backgroundColor = "#d3d3db";
        }
    }

    // 2. De Modus knoppen (state)
    // We selecteren alle knoppen met class 'bt', behalve de 'onoff' knop
    const modeButtons = document.querySelectorAll('.bt:not(#bt0)');
    
    modeButtons.forEach(btn => {
        // We checken of de ID van de knop (bijv. "btn-2") matcht met de state uit de JSON
        if (data.onoff === 1) {
            //console.log("device = on");
            if (btn.id === `bt${data.state}`) {
                //console.log("button should be green");
                btn.style.backgroundColor = "#1f8a07"; // Actieve modus is donker
            } else {
                //console.log("button should be grey");
                btn.style.backgroundColor = "#b9b9c1";  // Niet actieve modi blijven/worden grijs
            }
        } else
        {
            //console.log("device = off");
            // Als het apparaat uit staat (onoff is false), volg je de oude logica of een 'uit'-stijl
            if (btn.id === `bt${data.state}`) {
                btn.style.backgroundColor = "#5f5f6b";
            } else {
                btn.style.backgroundColor = "#b9b9c1";
            }
        }
    
    });

    // 3. De Sliders (hue, sat, level, etc.)
    // We updaten ze alleen als de gebruiker er niet aan zit (focus check)
    ['hue', 'sat', 'level'].forEach(key => {
        const slider = document.getElementById(key);
        const label = document.getElementById(`val-${key}`);
        if (slider && document.activeElement !== slider) {
            slider.value = data[key];
            // 1. Zorg dat de browser de styling toestaat (net als bij Hue)
            slider.style.webkitAppearance = 'none';
            slider.style.appearance = 'none';
        }
         if (label) {
            label.innerText = data[key];
        }        
        // Update Saturation gradient op basis van de HUE uit de DATA
         if (key === 'sat') {
            slider.style.background = `linear-gradient(to right, #ffffff, hsl(${data.hue}, 100%, 50%))`;
         }
    });
}

function updateLabel(id, waarde) {
    const label = document.getElementById(`val-${id}`);
    if (label) {
        label.innerText = waarde;
    }
}


</script>
  </body></html>

)rawliteral";