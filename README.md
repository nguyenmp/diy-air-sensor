# diy-air-sensor

Repository containing the code for my personal DIY air sensor.

My initial version was based on the hello_world example from esp_idf.  It uses the Panasonic SN-GCJA5 air sensor reading off of UART.  The code runs on the ESP32-PICO-DEV board which has a UART driver and header pins for easy dupont contacts/housing.

Note, you need the ESP-IDF framework to build this project: https://github.com/espressif/esp-idf/

To build:

```
source esp-idf/export.sh
cd diy-air-sensor/esp32_and_sn_gcja5/
idf.py -p /dev/cu.usbserial-1450 build flash monitor
```

You will probably need to change the path to the serial port for your machine.  I'm not sure if an automatic way to discover it.

If it works, this outputs the AQI for each PM range:

```
PM 1.0: 41, PM 2.5: 70 PM 10: 113
PC 0.5: 299, PC 1.0: 116, PC 2.5: 226, PC 5.0: 10, PC 7.5: 3, PC 10: 2
Status: 0
```

Notes were taken here: https://docs.google.com/document/d/1-P4PwsVpQIHHqxWCL41UcA0uUCThu2CVc9HfpG2WEOw/
