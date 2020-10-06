set(srcs "hello_world_main.c"
         "uart.c"
         "aqi.c"
         "bluetooth.c"
         "wifi.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ".")