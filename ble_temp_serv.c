
#include "ble_temp_serv.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"



/**@brief Connect event handler.
 *
 * @param[in]   p_temps     Temperature Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_temps_t * p_temps, ble_evt_t * p_ble_evt)
{
    p_temps->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Disconnect event handler.
 *
 * @param[in]   p_temps     Temperature Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_temps_t * p_temps, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_temps->conn_handle = BLE_CONN_HANDLE_INVALID;
}

// TODO: Change handler
/**@brief Write event handler.
 *
 * @param[in]   p_temps     Temperature Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_temps_t * p_temps, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    
   if ((p_evt_write->handle == p_temps->tempc_char_handles.value_handle) &&
       (p_evt_write->len == 1) &&
       (p_temps->led_write_handler != NULL))
   {
       p_temps->led_write_handler(p_temps, p_evt_write->data[0]);
   }
}


void ble_temps_on_ble_evt(ble_temps_t * p_temps, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_temps, p_ble_evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_temps, p_ble_evt);
            break;
            
        case BLE_GATTS_EVT_WRITE:
            on_write(p_temps, p_ble_evt);
            break;
            
        default:
            break;
    }
}


/**@brief Add Temperature level characteristic.
 *
 * @param[in]   p_temps        Temperature Service structure.
 * @param[in]   p_temps_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t tempc_char_add(ble_temps_t * p_temps, const ble_temps_init_t * p_temps_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    

    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    // TODO @notsure: Add Notify/Indicate? Should allow notifications to app based on temp change.
    char_md.p_char_user_desc  = TEMPC_DESC;
    // char_md.p_char_user_desc  = NULL;
    char_md.char_user_desc_max_size = sizeof(TEMPC_DESC);
    char_md.char_user_desc_size = sizeof(TEMPC_DESC);
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_temps->uuid_type;
    ble_uuid.uuid = TEMPS_UUID_TEMPC_CHAR;
    
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_temps->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_temps->tempc_char_handles);
}

/**@brief Add Button state characteristic.
 *
 * @param[in]   p_temps        Temperature Service structure.
 * @param[in]   p_temps_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t button_char_add(ble_temps_t * p_temps, const ble_temps_init_t * p_temps_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_temps->uuid_type;
    ble_uuid.uuid = TEMPS_UUID_BUTTON_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_temps->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_temps->button_char_handles);
}


uint32_t ble_temps_init(ble_temps_t * p_temps, const ble_temps_init_t * p_temps_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_temps->conn_handle       = BLE_CONN_HANDLE_INVALID;
    p_temps->led_write_handler = p_temps_init->led_write_handler;
    
    // Add base UUID to softdevice's internal list. 
    ble_uuid128_t base_uuid = TEMPS_UUID_BASE;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_temps->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    ble_uuid.type = p_temps->uuid_type;
    ble_uuid.uuid = TEMPS_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_temps->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    err_code = button_char_add(p_temps, p_temps_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    err_code  = tempc_char_add(p_temps, p_temps_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    return NRF_SUCCESS;
}

uint32_t ble_temps_on_button_change(ble_temps_t * p_temps, uint8_t button_state)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(button_state);
    
    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_temps->button_char_handles.value_handle;
    params.p_data = &button_state;
    params.p_len = &len;
    
    return sd_ble_gatts_hvx(p_temps->conn_handle, &params);    
}

