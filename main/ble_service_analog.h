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
    ANALOG_SERV,
    ANALOG_CHAR,
    ANALOG_CHAR_VAL,
    ANALOG_CHAR_CFG,

    ANALOG_SERV_NUM_ATTR,
};

#define ANALOG_SERVICE_UUID_LEN (16)

extern const esp_gatts_attr_db_t analog_service_gatt_db[ANALOG_SERV_NUM_ATTR];

typedef struct notify_params_t {
  esp_gatt_if_t gatt_if;
  uint16_t conn_id;
  uint16_t attr_handle;
} notify_params_t;

#endif // ANALOG_SERVICE_H_