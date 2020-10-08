set(srcs "main.c"
         "uart.c"
         "aqi.c"
         "bluetooth.c"
         "sn_gcja5.c"
         "wifi.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ".")