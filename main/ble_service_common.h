#ifndef COMMON_SERVICE_H_
#define COMMON_SERVICE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"

extern esp_ble_adv_data_t service_adv_data;
extern esp_ble_adv_data_t service_scan_rsp_data;

#endif // COMMON_SERVICE_H_