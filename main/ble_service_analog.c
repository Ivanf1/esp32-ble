#include "ble_service_analog.h"

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_DECLARATION_SIZE (sizeof(uint8_t))
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500

const static char *TAG = "EXAMPLE";

/* Service */
static const uint16_t GATTS_SERVICE_UUID_TEST = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_A = 0xFF01;

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

static const uint8_t char_prop_read_write_notify =
    ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

static const uint8_t analog_service_ccc[2] = {0x00, 0x00};

/* Full Database Description - Used to add attributes into the database */
const esp_gatts_attr_db_t analog_service_gatt_db[ANALOG_SERV_NUM_ATTR] = {
    // Service Declaration
    [ANALOG_SERV] = {{ESP_GATT_AUTO_RSP},
                     {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, sizeof(uint16_t),
                      sizeof(GATTS_SERVICE_UUID_TEST), (uint8_t *)&GATTS_SERVICE_UUID_TEST}},

    /* Characteristic Declaration */
    [ANALOG_CHAR] = {{ESP_GATT_AUTO_RSP},
                     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Characteristic Value */
    [ANALOG_CHAR_VAL] = {{ESP_GATT_AUTO_RSP},
                         {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_A, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                          GATTS_DEMO_CHAR_VAL_LEN_MAX, 0, NULL}},

    /* Client Characteristic Configuration Descriptor */
    [ANALOG_CHAR_CFG] = {{ESP_GATT_AUTO_RSP},
                         {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid,
                          ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, sizeof(uint16_t), sizeof(analog_service_ccc),
                          (uint8_t *)analog_service_ccc}},
};

// uint8_t analog_service_uuid[ANALOG_SERVICE_UUID_LEN] = {
//     /* LSB <--------------------------------------------------------------------------------> MSB */
//     // first uuid, 16bit, [12],[13] is the value
//     0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
// };

// ADC1 Channels
#define EXAMPLE_ADC1_CHAN0 ADC_CHANNEL_4

static int adc_raw;
static bool example_adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle);

static bool has_init_been_done = false;
adc_oneshot_unit_handle_t adc1_handle;

void analog_init(void) {
  //-------------ADC1 Init---------------//
  adc_oneshot_unit_init_cfg_t init_config1 = {
      .unit_id = ADC_UNIT_1,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

  //-------------ADC1 Config---------------//
  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH_DEFAULT,
      .atten = ADC_ATTEN_DB_11,
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));
}

void analog_sample(void *pvParameter) {
  notify_params_t *notify_params = ((notify_params_t *)pvParameter);

  adc_cali_handle_t adc1_cali_handle = NULL;
  example_adc_calibration_init(ADC_UNIT_1, ADC_ATTEN_DB_11, &adc1_cali_handle);

  if (!has_init_been_done) {
    analog_init();
    has_init_been_done = true;
  }
  static uint8_t *value_to_send;

  while (1) {
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, adc_raw);
    value_to_send = (uint8_t *)&adc_raw;
    esp_ble_gatts_send_indicate(notify_params->gatt_if, notify_params->conn_id, notify_params->attr_handle, sizeof(int),
                                value_to_send, false);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

static bool example_adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle) {
  adc_cali_handle_t handle = NULL;
  esp_err_t ret = ESP_FAIL;
  bool calibrated = false;

  if (!calibrated) {
    ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = atten,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
      calibrated = true;
    }
  }

  *out_handle = handle;
  if (ret == ESP_OK) {
    ESP_LOGI(TAG, "Calibration Success");
  } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
    ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
  } else {
    ESP_LOGE(TAG, "Invalid arg or no memory");
  }

  return calibrated;
}