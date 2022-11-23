#include "ble_service_devinfo.h"

#define CHAR_DECLARATION_SIZE (sizeof(uint8_t))

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ;

const uint16_t uuid_DEVINFO_SERV = ESP_GATT_UUID_DEVICE_INFO_SVC;
const uint16_t uuid_SYSTEM_ID = ESP_GATT_UUID_SYSTEM_ID;
const uint16_t uuid_MODEL_NUMBER_STR = ESP_GATT_UUID_MODEL_NUMBER_STR;
const uint16_t uuid_SERIAL_NUMBER_STR = ESP_GATT_UUID_SERIAL_NUMBER_STR;
const uint16_t uuid_FW_VERSION_STR = ESP_GATT_UUID_FW_VERSION_STR;
const uint16_t uuid_HW_VERSION_STR = ESP_GATT_UUID_HW_VERSION_STR;
const uint16_t uuid_SW_VERSION_STR = ESP_GATT_UUID_SW_VERSION_STR;
const uint16_t uuid_MANU_NAME = ESP_GATT_UUID_MANU_NAME;
const uint16_t uuid_IEEE_DATA = ESP_GATT_UUID_IEEE_DATA;
const uint16_t uuid_PNP_ID = ESP_GATT_UUID_PNP_ID;

#define DEVINFO_SERVICE_UUID_LEN (16)
static uint8_t devinfo_service_uuid[DEVINFO_SERVICE_UUID_LEN] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    // first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0A, 0x18, 0x00, 0x00,
};

// System ID characteristic
#define DEVINFO_SYSTEM_ID_LEN (8)
static uint8_t devInfoSystemId[DEVINFO_SYSTEM_ID_LEN] = {0, 0, 0, 0, 0, 0, 0, 0};

#define DEVINFO_STR_ATTR_LEN (20)

// Model Number String characteristic
static uint8_t devInfoModelNumber[DEVINFO_STR_ATTR_LEN + 1] = "DevKitC";

// Serial Number String characteristic
static uint8_t devInfoSerialNumber[DEVINFO_STR_ATTR_LEN + 1] = "blablabla";

// Firmware Revision String characteristic
static uint8_t devInfoFirmwareRev[DEVINFO_STR_ATTR_LEN + 1] = "1.0.0";

// Hardware Revision String characteristic
static uint8_t devInfoHardwareRev[DEVINFO_STR_ATTR_LEN + 1] = "3.2";

// Software Revision String characteristic
static uint8_t devInfoSoftwareRev[DEVINFO_STR_ATTR_LEN + 1] = "0.0.1";

// Manufacturer Name String characteristic
static uint8_t devInfoMfrName[DEVINFO_STR_ATTR_LEN + 1] = "Espressif";

// IEEE 11073-20601 Regulatory Certification Data List characteristic
// IEEE 11073 authoritative body values
#define DEVINFO_11073_BODY_EMPTY 0
#define DEVINFO_11073_BODY_IEEE 1
#define DEVINFO_11073_BODY_CONTINUA 2
#define DEVINFO_11073_BODY_EXP 254
static uint8_t defaultDevInfo11073Cert[] = {DEVINFO_11073_BODY_EXP, // authoritative body type
                                            0x00,                   // authoritative body structure type
                                                                    // authoritative body data follows below:
                                            'e', 'x', 'p', 'e', 'r', 'i', 'm', 'e', 'n', 't', 'a', 'l'};

// The length of this characteristic is not fixed
// static uint8_t *devInfo11073Cert = defaultDevInfo11073Cert;
// static uint8_t devInfo11073CertLen = sizeof(defaultDevInfo11073Cert);

// PnP ID characteristic
#define DEVINFO_PNP_ID_LEN (7)
static uint8_t devInfoPnpId[DEVINFO_PNP_ID_LEN] = {
    1,          // Vendor ID source (1=Bluetooth SIG)
    0xE5, 0x02, // Vendor ID (See https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers )
    0x00, 0x00, // Product ID (vendor-specific)
    0x10, 0x01  // Product version (JJ.M.N)
};

const esp_gatts_attr_db_t devinfo_service_gatt_db[DEVINFO_SERV_NUM_ATTR] = {
    // Service index
    [DEVINFO_SERV] = {{ESP_GATT_AUTO_RSP},
                      {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, sizeof(uint16_t),
                       sizeof(uuid_DEVINFO_SERV), (uint8_t *)&uuid_DEVINFO_SERV}},

    // System ID Declaration
    [DEVINFO_SYSTEM_ID_CHAR] = {{ESP_GATT_AUTO_RSP},
                                {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                                 CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // System ID Value
    [DEVINFO_SYSTEM_ID_VAL] = {{ESP_GATT_AUTO_RSP},
                               {ESP_UUID_LEN_16, (uint8_t *)&uuid_SYSTEM_ID, ESP_GATT_PERM_READ, ESP_GATT_MAX_ATTR_LEN,
                                DEVINFO_SYSTEM_ID_LEN, (uint8_t *)devInfoSystemId}},

    // Model Number String Declaration
    [DEVINFO_MODEL_NUMBER_STR_CHAR] = {{ESP_GATT_AUTO_RSP},
                                       {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                                        CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // Model Number String Value
    [DEVINFO_MODEL_NUMBER_STR_VAL] = {{ESP_GATT_AUTO_RSP},
                                      {ESP_UUID_LEN_16, (uint8_t *)&uuid_MODEL_NUMBER_STR, ESP_GATT_PERM_READ,
                                       ESP_GATT_MAX_ATTR_LEN, DEVINFO_STR_ATTR_LEN, (uint8_t *)devInfoModelNumber}},

    // Serial Number String Declaration
    [DEVINFO_SERIAL_NUMBER_STR_CHAR] = {{ESP_GATT_AUTO_RSP},
                                        {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                                         CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // Serial Number String Value
    [DEVINFO_SERIAL_NUMBER_STR_VAL] = {{ESP_GATT_AUTO_RSP},
                                       {ESP_UUID_LEN_16, (uint8_t *)&uuid_SERIAL_NUMBER_STR, ESP_GATT_PERM_READ,
                                        ESP_GATT_MAX_ATTR_LEN, DEVINFO_STR_ATTR_LEN, (uint8_t *)devInfoSerialNumber}},

    // Firmware Revision String Declaration
    [DEVINFO_FW_VERSION_STR_CHAR] = {{ESP_GATT_AUTO_RSP},
                                     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                                      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // Firmware Revision String Value
    [DEVINFO_FW_VERSION_STR_VAL] = {{ESP_GATT_AUTO_RSP},
                                    {ESP_UUID_LEN_16, (uint8_t *)&uuid_FW_VERSION_STR, ESP_GATT_PERM_READ,
                                     ESP_GATT_MAX_ATTR_LEN, DEVINFO_STR_ATTR_LEN, (uint8_t *)devInfoFirmwareRev}},

    // Hardware Revision String Declaration
    [DEVINFO_HW_VERSION_STR_CHAR] = {{ESP_GATT_AUTO_RSP},
                                     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                                      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // Hardware Revision String Value
    [DEVINFO_HW_VERSION_STR_VAL] = {{ESP_GATT_AUTO_RSP},
                                    {ESP_UUID_LEN_16, (uint8_t *)&uuid_HW_VERSION_STR, ESP_GATT_PERM_READ,
                                     ESP_GATT_MAX_ATTR_LEN, DEVINFO_STR_ATTR_LEN, (uint8_t *)devInfoHardwareRev}},

    // Software Revision String Declaration
    [DEVINFO_SW_VERSION_STR_CHAR] = {{ESP_GATT_AUTO_RSP},
                                     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                                      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // Software Revision String Value
    [DEVINFO_SW_VERSION_STR_VAL] = {{ESP_GATT_AUTO_RSP},
                                    {ESP_UUID_LEN_16, (uint8_t *)&uuid_SW_VERSION_STR, ESP_GATT_PERM_READ,
                                     ESP_GATT_MAX_ATTR_LEN, DEVINFO_STR_ATTR_LEN, (uint8_t *)devInfoSoftwareRev}},

    // Manufacturer Name String Declaration
    [DEVINFO_MANU_NAME_CHAR] = {{ESP_GATT_AUTO_RSP},
                                {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                                 CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // Manufacturer Name String Value
    [DEVINFO_MANU_NAME_VAL] = {{ESP_GATT_AUTO_RSP},
                               {ESP_UUID_LEN_16, (uint8_t *)&uuid_MANU_NAME, ESP_GATT_PERM_READ, ESP_GATT_MAX_ATTR_LEN,
                                DEVINFO_STR_ATTR_LEN, (uint8_t *)devInfoMfrName}},

    // IEEE 11073-20601 Regulatory Certification Data List Declaration
    [DEVINFO_IEEE_DATA_CHAR] = {{ESP_GATT_AUTO_RSP},
                                {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                                 CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // IEEE 11073-20601 Regulatory Certification Data List Value
    [DEVINFO_IEEE_DATA_VAL] = {{ESP_GATT_AUTO_RSP},
                               {ESP_UUID_LEN_16, (uint8_t *)&uuid_IEEE_DATA, ESP_GATT_PERM_READ, ESP_GATT_MAX_ATTR_LEN,
                                sizeof(defaultDevInfo11073Cert), (uint8_t *)defaultDevInfo11073Cert}},

    // PnP ID Declaration
    [DEVINFO_PNP_ID_CHAR] = {{ESP_GATT_AUTO_RSP},
                             {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                              CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // PnP ID Value
    [DEVINFO_PNP_ID_VAL] = {{ESP_GATT_AUTO_RSP},
                            {ESP_UUID_LEN_16, (uint8_t *)&uuid_PNP_ID, ESP_GATT_PERM_READ, ESP_GATT_MAX_ATTR_LEN,
                             DEVINFO_PNP_ID_LEN, (uint8_t *)devInfoPnpId}},

};

/* The length of adv data must be less than 31 bytes */
const esp_ble_adv_data_t devinfo_service_adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006, // slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, // slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0,       // TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, // test_manufacturer,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(devinfo_service_uuid),
    .p_service_uuid = devinfo_service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
const esp_ble_adv_data_t devinfo_service_scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,       // TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(devinfo_service_uuid),
    .p_service_uuid = devinfo_service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};