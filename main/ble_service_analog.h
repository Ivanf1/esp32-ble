#ifndef ANALOG_SERVICE_H_
#define ANALOG_SERVICE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"

void analog_sample(void* pvParameter);

/* Attributes State Machine */
enum {
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    HRS_IDX_NB,
};

#define ANALOG_SERVICE_UUID_LEN (16)

extern const esp_gatts_attr_db_t analog_service_gatt_db[HRS_IDX_NB];

typedef struct notify_params_t {
  esp_gatt_if_t gatt_if;
  uint16_t conn_id;
  uint16_t attr_handle;
} notify_params_t;

#endif // ANALOG_SERVICE_H_