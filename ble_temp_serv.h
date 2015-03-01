

/** @file
 *
 * @defgroup ble_sdk_srv_temps Temperature Service
 * @{
 * @ingroup ble_sdk_srv
 * @brief Temperature Service module.
 *
 * @details 
 TODO: Write details for this service:
						Service covers reading temp values, configuration of min temp warning and max temp warning.
						
 This module implements the LEDButton Service with the Battery Level characteristic.
 *          During initialization it adds the LEDButton Service and Battery Level characteristic
 *          to the BLE stack datatempse. Optionally it can also add a Report Reference descriptor
 *          to the Battery Level characteristic (used when including the LEDButton Service in
 *          the HID service).
 *
 *          If specified, the module will support notification of the Battery Level characteristic
 *          through the ble_temps_battery_level_update() function.
 *          If an event handler is supplied by the application, the LEDButton Service will
 *          generate LEDButton Service events to the application.
 *
 * @note The application must propagate BLE stack events to the LEDButton Service module by calling
 *       ble_temps_on_ble_evt() from the from the @ref ble_stack_handler callback.
 */

#ifndef BLE_TEMPS_H__
#define BLE_TEMPS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

#define TEMPS_UUID_BASE {0x72, 0x6F, 0x74, 0x69, 0x6E, 0x6F, 0x6D, 0x74, 0x6E, 0x61, 0x6C, 0x70, 0x00, 0x00, 0x00, 0x00}
#define TEMPS_UUID_SERVICE 0x746D					/* TM - Temperature Monitoring. */
/** TOCHANGE: */
#define TEMPS_UUID_LED_CHAR 0x1525
#define TEMPS_UUID_BUTTON_CHAR 0x1524
// Something, something, something, current temp?
#define TEMPS_UUID_TEMPC_CHAR   0x7463              /* TC - Temperature, Current */
#define TEMPS_UUID_TEMPR_CHAR   0x7472              /* TR - Temperature Range */

static uint8_t TEMPC_DESC[] = {"Current Temperature (Celcius)"};
/** END TOCHANGE */


// Forward declaration of the ble_temps_t type. 
typedef struct ble_temps_s ble_temps_t;

/**@brief Temperature Service event handler type. */
typedef void (*ble_temps_led_write_handler_t) (ble_temps_t * p_temps, uint8_t new_state);

/**@brief Temperature Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_temps_led_write_handler_t   led_write_handler;                   
} ble_temps_init_t;


/**@brief Temperature Service structure. This contains various status information for the service. */
typedef struct ble_temps_s
{         
    uint16_t                     service_handle;                 
    ble_gatts_char_handles_t     tempc_char_handles;          
    // ble_gatts_char_handles_t     tempr_char_handles;
    ble_gatts_char_handles_t     button_char_handles;
    uint8_t                      uuid_type;
    uint8_t                      current_temp;             
    uint16_t                     conn_handle;  
    bool                         is_notifying;
    ble_temps_led_write_handler_t  led_write_handler;
} ble_temps_t;


/**@brief Initialize the Temperature Service.
 *
 * @param[out]  p_temps       
 * @param[in]   p_temps_init  
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_temps_init(ble_temps_t * p_temps, const ble_temps_init_t * p_temps_init);

/**@brief Temperature Service BLE stack event handler.
 *
 * @details Handles all events from the BLE stack of interest to the Temperature Service.
 *
 * @param[in]   p_temps     Temperature Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_temps_on_ble_evt(ble_temps_t * p_temps, ble_evt_t * p_ble_evt);

// TODO: Still to notify
/**@brief Handler to notify the Temperature service on button presses.
 *
 * @param[in]   p_temps     Temperature Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
uint32_t ble_temps_on_button_change(ble_temps_t * p_temps, uint8_t button_state);

#endif // BLE_TEMPS_H__

/** @} */
