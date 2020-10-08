/* Based on Hello World Example from esp-idf */
#include "uart.h"
#include "aqi.h"
#include "bluetooth.h"
#include "wifi.h"
#include "sn_gcja5.h"

void app_main(void)
{
    printf("Hello world!\n");
    const int uart_num = UART_NUM_2;
    QueueHandle_t uart_queue;
    init_uart(uart_num, &uart_queue);

    init_bluetooth();
    init_wifi();

    int counter = 0;
    while (true) {
        // Sleep for 1 second
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        printf("Counter: %d\n", counter);
        counter += 1;

        // Read data from UART.
        uint8_t data[128];
        int length = 0;
        ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
        length = uart_read_bytes(uart_num, data, length, 100);
        printf("Read %d bytes\n", length);
        if (length >= 32) {
            parse_uart(data);
        } else {
            printf("Ignoring event, insufficient data...\n");
        }
    }
}
