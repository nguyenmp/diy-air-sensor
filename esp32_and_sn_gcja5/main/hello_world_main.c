/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <assert.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

/* Assumes 32 bytes, https://b2b-api.panasonic.eu/file_stream/pids/fileversion/8814 */
void parse_uart(uint8_t data[])
{
    // STX fixed value
    assert(data[0] == 2);

    // PM values (ug/m3)
    int pm_1_0 = data[1] + (data[2] << 8) + (data[3] << 16) + (data[4] << 24);
    int pm_2_5 = data[5] + (data[6] << 8) + (data[7] << 16) + (data[8] << 24);
    int pm_10 = data[9] + (data[10] << 8) + (data[11] << 16) + (data[12] << 24);

    // Particle counts (#, baseless)
    int pc_0_5 = data[13] + (data[14] << 8);
    int pc_1_0 = data[15] + (data[16] << 8);
    int pc_2_5 = data[17] + (data[18] << 8);
    assert(data[19] == 0);
    assert(data[20] == 0);
    int pc_5_0 = data[21] + (data[22] << 8);
    int pc_7_5 = data[23] + (data[24] << 8);
    int pc_10 = data[25] + (data[26] << 8);
    assert(data[27] == 0);
    assert(data[28] == 0);

    // Status
    int status = data[29];

    // XOR between data 2~30
    int fcc = data[30];
    int xor = 0;
    for (int i = 1; i < 30; i++) {
        xor ^= data[i];
    }
    assert(fcc == xor);

    // EXT fixed value
    assert(data[31] == 3);

    // Print human values
    printf("PM 1.0: %d, PM 2.5: %d PM 10: %d\n", pm_1_0, pm_2_5, pm_10);
    printf(
        "PC 0.5: %d, PC 1.0: %d, PC 2.5: %d, PC 5.0: %d, PC 7.5: %d, PC 10: %d\n",
        pc_0_5, pc_1_0, pc_2_5, pc_5_0, pc_7_5, pc_10
    );
    printf("Status: %d\n", status);
    fflush(stdout);
}

void app_main(void)
{
    printf("Hello world!\n");

    // Configure UART parameters
    const int uart_num = UART_NUM_2;
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
    };
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, UART_PIN_NO_CHANGE, 22, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Setup UART buffered IO with event queue
    // Install UART driver using an event queue here
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, \
                                            uart_buffer_size, 10, &uart_queue, 0));

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
