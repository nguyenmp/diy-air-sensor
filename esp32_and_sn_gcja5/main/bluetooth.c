#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_nimble_hci.h" 
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

const char TAG[] = "bluetooth.c";

// This is the task run by the NimBLE background thread when passed to
// nimble_port_freertos_init.  It should call nimble_port_run then
// nimble_port_freertos_deinit.
void ble_host_task(void *param)
{
     // nimble_port_run will block until nimble_port_stop() is called
     nimble_port_run();

     // "If nimble_port_freertos_init() is used during initialization, then
     // nimble_port_freertos_deinit() should be called in the host task
     // after nimble_port_run()."  -- ESP API docs
     nimble_port_freertos_deinit();
}

// This is specifically loading the drivers and starting the bits
// There is no real application specific code here, just generic boilerplate
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/nimble/index.html
void start_bluetooth_stack() {
    esp_err_t ret;

    // Initialize NVS flash using nvs_flash_init() API. This is
    // because ESP controller uses NVS during initialization.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Finished nvs_flash_init");

    // Call esp_nimble_hci_and_controller_init() to initialize ESP controller
    // as well as transport layer. This will also link the host and controller
    // modules together. Alternatively, if ESP controller is already
    // initialized, then esp_nimble_hci_init() can be called for the remaining
    // initialization.
    ret = esp_nimble_hci_and_controller_init();
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Finished esp_nimble_hci_and_controller_init");

    // Initialize the host stack using nimble_port_init.
    // source code shows it returns void so no error checking here...
    nimble_port_init();
    ESP_LOGI(TAG, "Finished nimble_port_init");

    // Initialize the required NimBLE host configuration parameters and callbacks

    // Perform application specific tasks/initialization

    // Run the thread for host stack using nimble_port_freertos_init
    nimble_port_freertos_init(ble_host_task);
    ESP_LOGI(TAG, "Finished nimble_port_freertos_init");
}

void init_bluetooth() {
    start_bluetooth_stack();
    start_bluetooth_advertising();
}