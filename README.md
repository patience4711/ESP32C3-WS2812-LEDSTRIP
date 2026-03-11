This is software for a very smart led controller with built-in timers and dim function. It listens to voice commands via esp-rainmaker and google home. It can also work stand-alone and with its own timekeeping and timers you can make complex automations. It is meant for led lights, e.g. a ledstrip. You can apply it in many cases.
For instance, you can wind the strip around a hollow plastic piece of pipe to create a bulb that you can mount in a lantern or armature.
This way you can build a very intelligent and low-energy garden lamp that switches on about sunset and goes out at sunrise. 

Please visit the [WIKI](https://github.com/patience4711/ESP32C3-SMART-LIGHT/wiki) for more information

The main features are:
- easy to connect to wifi via esp rainmaker app on your phone
- controllable via webui, voice commands (google home), mosquitto, fysical button (optional)
- dimmable with soft-on and off
- 4 built-in timers that can switch to adjustable dim levels.

### downloads
2026-03-11 There is a new binary available ( see change log)<br>
[ESP32C3-RM-DIMMER-V0_3a](https://github.com/patience4711/ESP32C3-SMART-LIGHT/blob/main/RMakerdimmer_v3.ino.merged.bin)

### Resetting the device
- Press and Hold the Boot button for more than 3 seconds and then release to reset Wi-Fi configuration.
- Press and Hold the Boot button for more than 10 seconds and then release to reset to factory defaults.
## CHANGE LOG
v03a:
- improved the mosquitto functionallity.
- fixed some imperfections in the webinterface.
