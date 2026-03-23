This is software for a very smart controller for a ws2812 ledstrip. It has 4 built-in timers that can switch a defined color light. It listens to voice commands via esp-rainmaker and google home. It can also work stand-alone and with its own timekeeping and timers you can make complex automations. 
For instance, you can wind the strip around a hollow plastic piece of pipe to create a bulb that you can mount in a lantern or armature.

Please visit the [WIKI](https://github.com/patience4711/ESP32C3-S/wiki) for more information

The main features are:
- easy to connect to wifi via esp rainmaker app on your phone
- controllable via webui, voice commands (google home), mosquitto, fysical button (optional)
- 4 built-in timers that can switch to adjustable dim levels.
- color control with sliders for hue and saturation

### downloads
2026-03-11 There is a new binary available ( see change log)<br>
[ESP32C3-WS2812-V0_1](https://github.com/patience4711/ESP32C3-WS2812-LEDSTRIP/blob/main/RMakerLedstrip_v0_1.ino.merged.bin)<br><br>
![mainpage](https://github.com/user-attachments/assets/6130b48e-c553-4d50-879f-d09c467e2eea)

### Resetting the device
- Press and Hold the Boot button for more than 3 seconds and then release to reset Wi-Fi configuration.
- Press and Hold the Boot button for more than 10 seconds and then release to reset to factory defaults.
## CHANGE LOG
v0_1:
- solved problems as to rainmker connection
- improved communication via mqtt and cloud

