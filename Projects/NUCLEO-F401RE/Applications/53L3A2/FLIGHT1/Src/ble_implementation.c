/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ble_implementation.c
  * @author  System Research & Applications Team - Catania Lab.
  * @brief   BLE Implementation header template file.
  *          This file should be copied to the application folder and renamed
  *          to ble_implementation.c.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "ble_manager.h"
#include "main.h"

__weak void ble_set_custom_advertise_data(uint8_t *manuf_data);
__weak void set_board_name(void);
__weak void enable_extended_configuration_command(void);

#if (BLUE_CORE != BLUE_WB)
__weak void reset_ble_manager(void);
#endif /* (BLUE_CORE != BLUE_WB) */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private functions ---------------------------------------------------------*/

/** @brief Initialize the BlueNRG stack and services
  * @param  None
  * @retval None
  */
void bluetooth_init(void)
{
  /* BlueNRG stack setting */
  ble_stack_value.config_value_offsets                    = CONFIG_VALUE_OFFSETS;
  ble_stack_value.config_value_length                     = CONFIG_VALUE_LENGTH;
  ble_stack_value.gap_roles                               = GAP_ROLES;
  ble_stack_value.io_capabilities                         = IO_CAPABILITIES;
  ble_stack_value.authentication_requirements             = AUTHENTICATION_REQUIREMENTS;
  ble_stack_value.mitm_protection_requirements            = MITM_PROTECTION_REQUIREMENTS;
  ble_stack_value.secure_connection_support_option_code   = SECURE_CONNECTION_SUPPORT_OPTION_CODE;
  ble_stack_value.secure_connection_keypress_notification = SECURE_CONNECTION_KEYPRESS_NOTIFICATION;

  /* Use BLE Random Address */
  ble_stack_value.own_address_type = ADDRESS_TYPE;

  /* Set the BLE Board Name */
  set_board_name();

  /* En_High_Power Enable High Power mode.
     High power mode should be enabled only to reach the maximum output power.
     Values:
     - 0x00: Normal Power
     - 0x01: High Power */
  ble_stack_value.enable_high_power_mode = ENABLE_HIGH_POWER_MODE;

  /* Values: 0x00 ... 0x31 - The value depends on the device */
  ble_stack_value.power_amplifier_output_level = POWER_AMPLIFIER_OUTPUT_LEVEL;

  /* BlueNRG services setting */
  ble_stack_value.enable_config    = ENABLE_CONFIG;
  ble_stack_value.enable_console   = ENABLE_CONSOLE;
  ble_stack_value.enable_ext_config = ENABLE_EXT_CONFIG;

  /* For Enabling the Secure Connection */
  ble_stack_value.enable_secure_connection = ENABLE_SECURE_CONNECTION;
  /* Default Secure PIN */
  ble_stack_value.secure_pin = SECURE_PIN;
  /* For creating a Random Secure PIN */
  ble_stack_value.enable_random_secure_pin = ENABLE_RANDOM_SECURE_PIN;

  /* Advertising policy for filtering (white list related) */
  ble_stack_value.advertising_filter = ADVERTISING_FILTER;

  /* Used platform */
  ble_stack_value.board_id = BLE_MANAGER_USED_PLATFORM;

  if (ble_stack_value.enable_secure_connection)
  {
    /* Using the Secure Connection, the Rescan should be done by BLE chip */
    ble_stack_value.force_rescan = 0;
  }
  else
  {
    ble_stack_value.force_rescan = 1;
  }

  init_ble_manager();
}

/**
  * @brief  Set Board Name.
  * @param  None
  * @retval None
  */
__weak void set_board_name(void)
{
  sprintf(ble_stack_value.board_name, "%s%c%c%c", "BLEM",
          BLE_VERSION_FW_MAJOR,
          BLE_VERSION_FW_MINOR,
          BLE_VERSION_FW_PATCH);
}

/**
  * @brief  Enable/Disable Extended Configuration Command.
  * @param  None
  * @retval None
  */
__weak void enable_extended_configuration_command(void)
{
  /* Enable/Disable Board Report Extended configuration commands */
  ble_extended_configuration_value.stm32_uid = ENABLE_STM32_UID_EXT_CONFIG;
  ble_extended_configuration_value.info = ENABLE_INFO_EXT_CONFIG;
  ble_extended_configuration_value.help = ENABLE_HELP_EXT_CONFIG;
  ble_extended_configuration_value.version_fw = ENABLE_VERSION_FW_EXT_CONFIG;
  ble_extended_configuration_value.power_status = ENABLE_POWER_STATUS_EXT_CONFIG;

  /* Enable/Disable Board Security Extended configuration commands */
  ble_extended_configuration_value.change_secure_pin = ENABLE_CHANGE_SECURE_PIN_EXT_CONFIG;
  ble_extended_configuration_value.clear_secure_data = ENABLE_CLEAR_SECURE_DATA_BASE_EXT_CONFIG;
  ble_extended_configuration_value.read_certificate = ENABLE_READ_CERTIFICATE_EXT_CONFIG;
  ble_extended_configuration_value.set_certificate = ENABLE_SET_CERTIFICATE_EXT_CONFIG;

  /* Enable/Disable Board Control Extended configuration commands */
  ble_extended_configuration_value.reboot_on_dfu_mode_command = ENABLE_REBOOT_ON_DFU_MODE_EXT_CONFIG;
  ble_extended_configuration_value.power_off = ENABLE_POWER_OFF_EXT_CONFIG;
  ble_extended_configuration_value.read_banks = ENABLE_READ_BANKS_FW_ID_EXT_CONFIG;
  ble_extended_configuration_value.banks_swap = ENABLE_BANKS_SWAP_EXT_CONFIG;

  /* Enable/Disable Board Settings Extended configuration commands */
  ble_extended_configuration_value.set_name = ENABLE_SET_NAME_EXT_CONFIG;
  ble_extended_configuration_value.read_custom_command = ENABLE_READ_CUSTOM_COMMAND_EXT_CONFIG;
  ble_extended_configuration_value.set_date = ENABLE_SET_DATE_EXT_CONFIG;
  ble_extended_configuration_value.set_time = ENABLE_SET_TIME_EXT_CONFIG;
  ble_extended_configuration_value.set_wifi = ENABLE_SET_WIFI_EXT_CONFIG;
}

/**
  * @brief  Custom Service Initialization.
  * @param  None
  * @retval None
  */
void ble_init_custom_service(void)
{
  /**
    * User can added here the custom service initialization for the selected BLE features.
    * For example for the environmental features:
  */
  /* Characteristc allocation for LED Bluetooth feature */
  ble_manager_add_char(ble_init_led_service());
  /* Characteristc allocation for objects detection features */
  ble_manager_add_char(ble_init_objects_detection_service());
}

/**
  * @brief  Set Custom Advertize Data.
  * @param  uint8_t *manuf_data: Advertize Data
  * @retval None
  */
__weak void ble_set_custom_advertise_data(uint8_t *manuf_data)
{
#ifndef BLE_MANAGER_SDKV2
  /**
    * For only SDKV1, user can add here the custom advertize data setting for the selected BLE features.
    * For example for the environmental features:
    *
    * ble_set_custom_advertise_data(manuf_data);
    */

#else /* BLE_MANAGER_SDKV2 */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD1] = 0xFF; /* Custom Firmware */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD2] = 0x00;
  manuf_data[BLE_MANAGER_CUSTOM_FIELD3] = 0x00;
  manuf_data[BLE_MANAGER_CUSTOM_FIELD4] = 0x00;
#endif /* BLE_MANAGER_SDKV2 */
}

#if (BLUE_CORE != BLUE_WB)
/**
  * @brief  reset_ble_manager
  * @param  None
  * @retval None
  */
__weak void reset_ble_manager(void)
{
  BLE_MANAGER_PRINTF("\r\nReset BleManager (It is a week function)\r\n\r\n");
}
#endif /* (BLUE_CORE != BLUE_WB) */
