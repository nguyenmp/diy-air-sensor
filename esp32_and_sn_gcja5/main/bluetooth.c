// https://github.com/espressif/esp-idf/blob/8bc19ba893e5544d571a753d82b44a84799b94b1/examples/bluetooth/bluedroid/ble/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"

#define PROFILE_NUM 1
#define PROFILE_A_APP_ID 0
#define TEST_DEVICE_NAME            "ESP_GATTS_DEMO"

static uint8_t adv_service_uuid128[32] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xEE, 0x00, 0x00, 0x00,
    //second uuid, 32bit, [12], [13], [14], [15] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

///Declare the static function
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);


struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = 0,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};


static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = false,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    //.min_interval = 0x0006,
    //.max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static const char* TAG = "my_bluetooth_c";

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(TAG, "Received GATTS Event: %d", event);
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        ESP_LOGI(TAG, "Received a register event");

        // pull out variables from param
        uint16_t app_id = param->reg.app_id;
        esp_gatt_status_t status = param->reg.status;

        if (status == ESP_GATT_OK) {
            ESP_LOGI(TAG, "Status was ESP_GATT_OK, asigning interface for app_id: %d", app_id);
            gl_profile_tab[app_id].gatts_if = gatts_if;
        } else {
            ESP_LOGE(TAG, "Reg app failed, app_id %04x, status %d\n", app_id, status);
            return;
        }
    }

    /* If the gatts_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    ESP_LOGI(TAG, "Delegating to matching profiles");
    for (int idx = 0; idx < PROFILE_NUM; idx++) {
        if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                gatts_if == gl_profile_tab[idx].gatts_if) {
            ESP_LOGI(TAG, "Interface matched app_id: %d", idx);
            if (gl_profile_tab[idx].gatts_cb) {
                ESP_LOGI(TAG, "gatts_callback exists, delegating...");
                gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
            } else {
                ESP_LOGI(TAG, "gatts_callback does not exist, ignoring...");
            }
        }
    }

}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    ESP_LOGI(TAG, "Received GAP Event: %d", event);
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_SCAN_RESULT_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SCAN_RESULT_EVT");
            break;
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_ADV_START_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SCAN_START_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_AUTH_CMPL_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_AUTH_CMPL_EVT");
            break;
        case ESP_GAP_BLE_KEY_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_KEY_EVT");
            break;
        case ESP_GAP_BLE_SEC_REQ_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SEC_REQ_EVT");
            break;
        case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_PASSKEY_NOTIF_EVT");
            break;
        case ESP_GAP_BLE_PASSKEY_REQ_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_PASSKEY_REQ_EVT");
            break;
        case ESP_GAP_BLE_OOB_REQ_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_OOB_REQ_EVT");
            break;
        case ESP_GAP_BLE_LOCAL_IR_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_LOCAL_IR_EVT");
            break;
        case ESP_GAP_BLE_LOCAL_ER_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_LOCAL_ER_EVT");
            break;
        case ESP_GAP_BLE_NC_REQ_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_NC_REQ_EVT");
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT");
            break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT");
            break;
        case ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT");
            break;
        case ESP_GAP_BLE_SET_CHANNELS_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SET_CHANNELS_EVT");
            break;
        case ESP_GAP_BLE_EVT_MAX:
            ESP_LOGI(TAG, "ESP_GAP_BLE_EVT_MAX");
            break;
        default:
            ESP_LOGE(TAG, "Unknown");
            break;
    }
}

void init_bluetooth() {
    esp_err_t ret;

    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s initialize controller failed\n", __func__);
        assert(false);
    } else {
        printf("Successfully ran esp_bt_controller_init\n");
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed\n", __func__);
        return;
    } else {
        printf("Successfully ran esp_bt_controller_enable\n");
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluetooth failed\n", __func__);
        return;
    } else {
        printf("Successfully ran esp_bluedroid_init\n");
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluetooth failed\n", __func__);
        return;
    } else {
        printf("Successfully ran esp_bluedroid_enable\n");
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret){
        ESP_LOGE(TAG, "gatts register error, error code = %x", ret);
        return;
    } else {
        printf("Successfully ran esp_ble_gatts_register_callback\n");
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(TAG, "gap register error, error code = %x", ret);
        return;
    } else {
        printf("Successfully ran esp_ble_gap_register_callback\n");
    }

    ret = esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
        return;
    } else {
        printf("Successfully ran esp_ble_gatts_app_register\n");
    }

}