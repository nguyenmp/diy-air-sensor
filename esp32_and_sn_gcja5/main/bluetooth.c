#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_nimble_hci.h" 
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_gatt.h"  // nimble API
#include "host/ble_hs_adv.h"  // for ble_hs_adv_fields
#include "host/ble_gap.h" // for ble_gap_adv_set_fields

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

int gap_event_handler(struct ble_gap_event *event, void *arg)
{
    ESP_LOGI(TAG, "Received GAP event of type: %d", event->type);
    // It's unknown what the return should be but
    // the typedef says it should return a int
    return 0;
}

// https://mynewt.apache.org/latest/tutorials/ble/bleprph/bleprph-sections/bleprph-adv.html
void start_bluetooth_advertising() {
    struct ble_hs_adv_fields fields;
    int rc;
    const char *bleprph_device_name = "My ESP32 Hello";

    /* Set the advertisement data included in our advertisements. */
    memset(&fields, 0, sizeof fields);
    fields.name = (uint8_t *)bleprph_device_name;
    fields.name_len = strlen(bleprph_device_name);
    fields.name_is_complete = 1;
    rc = ble_gap_adv_set_fields(&fields);
    if (rc == BLE_HS_ENOTSYNCED) {
        ESP_LOGE(TAG, "BLE_HS_ENOTSYNCED; rc=%d", rc);
        return;
    } if (rc != 0) {
        ESP_LOGE(TAG, "error setting advertisement data; rc=%d", rc);
        return;
    }

    /* Begin advertising. */
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(
        BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER,
        &adv_params, gap_event_handler, NULL
    );
    if (rc != 0) {
        ESP_LOGE(TAG, "error enabling advertisement; rc=%d", rc);
        return;
    }

    ESP_LOGI(TAG, "Finished starting advertising...");
}

#define PRIMARY_SERVICE_UUID BLE_UUID128_DECLARE( \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#define PRIMARY_CHARACTERISTIC_UUID BLE_UUID128_DECLARE( \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1)

int rand_num = 42;

int write_from_om(struct ble_gatt_access_ctxt *ctxt, void *dest, uint16_t size) {
    // Validate that we aren't given data of the wrong size
    uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);
    if (om_len != (sizeof rand_num)) {
        ESP_LOGE(TAG, "Wrong size for write: %d (expected %d)", om_len, (sizeof rand_num));
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    // Perform the actual memory copy, checking that we copied everything fully
    uint16_t actually_copied_len = 0;
    int rc = ble_hs_mbuf_to_flat(ctxt->om, &rand_num, om_len, &actually_copied_len);
    if (actually_copied_len != om_len) {
        ESP_LOGW(TAG, "Did not fully copy content: %d of %d", actually_copied_len, om_len);
    }
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_hs_mbuf_to_flat failed: %d", rc);
        return BLE_ATT_ERR_UNLIKELY;
    }

    // Success!
    return 0;
}

int characteristic_access_callback(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    ESP_LOGI(TAG, "Accessed a characteristic with operation: %d", ctxt->op);

    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
        {
            ESP_LOGI(TAG, "BLE_GATT_ACCESS_OP_READ_CHR");
            int rc = os_mbuf_append(ctxt->om, &rand_num, sizeof rand_num);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
        {
            ESP_LOGI(TAG, "BLE_GATT_ACCESS_OP_WRITE_CHR");

            return write_from_om(ctxt, &rand_num, sizeof rand_num);
        }
        case BLE_GATT_ACCESS_OP_READ_DSC:
        case BLE_GATT_ACCESS_OP_WRITE_DSC:
            ESP_LOGW(TAG, "Unsupported operation");
            return 1;
        default:
            ESP_LOGE(TAG, "Unknown characteristic access operation: %d", ctxt->op);
            return 1;
    }

    // This means a successful read
    return 0;
}

// Struct as documented in:
// * https://mynewt.apache.org/latest/network/ble_hs/ble_gatts.html#c.ble_gatt_svc_def
// * https://mynewt.apache.org/latest/network/ble_hs/ble_gatts.html#c.ble_gatt_chr_def
const struct ble_gatt_svc_def gatt_services[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = PRIMARY_SERVICE_UUID,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = PRIMARY_CHARACTERISTIC_UUID,
                .access_cb = characteristic_access_callback,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
            },
            {
                0, /* No more characteristics in this service. */
            },
        },
    },
    {
        0, // No more services in this array
    },
};

void host_sync_callback(void) {
    // This is application specific code, where we
    // choose what to advertise and broadcast data
    // https://mynewt.apache.org/latest/network/index.html#ble-user-guide
    start_bluetooth_advertising();
}

void initialize_services(void) {
    // This function was undocumented in the official tutorial:
    // https://mynewt.apache.org/latest/tutorials/ble/bleprph/bleprph-sections/bleprph-svc-reg.html
    // However, it was in the following example:
    // https://github.com/apache/mynewt-core/blob/master/apps/bsnprph/src/gatt_svr.c#L115-L131
    // Without the call to ble_gatts_count_cfg, you will not see the service addition
    int rc = ble_gatts_count_cfg(gatt_services);
    if (rc != 0) {
        ESP_LOGE(TAG, "error ble_gatts_count_cfg; rc=%d", rc);
        assert(rc == 0);
    } else {
        ESP_LOGI(TAG, "Finished ble_gatts_count_cfg");
    }

    rc = ble_gatts_add_svcs(gatt_services);
    if (rc != 0) {
        ESP_LOGE(TAG, "error ble_gatts_add_svcs; rc=%d", rc);
        assert(rc == 0);
    } else {
        ESP_LOGI(TAG, "Finished ble_gatts_add_svcs");
    }

    ble_gatts_start();
}

void init_bluetooth() {
    // Hook up the OS and drivers and stuff
    start_bluetooth_stack();

    // Load up our services which is a table of characteristics we are sharing
    initialize_services();

    // We can only start advertising once the host config has
    // been synced, or else we receive BLE_HS_ENOTSYNCED
    // https://mynewt.apache.org/latest/tutorials/ble/ibeacon.html
    ble_hs_cfg.sync_cb = host_sync_callback;
}